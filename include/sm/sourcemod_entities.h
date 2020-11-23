#pragma once

#include <server_class.h>
#include <IHandleEntity.h>
#include "variant_t.h"
#include <stdexcept>
#include <vector>
#include <span>

namespace sm {
	inline namespace entities {
		inline const char* GetEdictClassname(edict_t* ed)
		{
			return gamehelpers->GetEntityClassname(ed);
		}

		template<class T>
		T &EntData(AutoEntity<CBaseEntity *> pEntity, unsigned short offset, int size=sizeof(T))
		{
			assert(pEntity != nullptr);
			T *data = (T *)(reinterpret_cast<intptr_t>(static_cast<CBaseEntity*>(pEntity)) + offset);
			return *data;
		}
		template<class T>
		const T &GetEntData(AutoEntity<CBaseEntity*> pEntity, unsigned short offset, int size=sizeof(T))
		{
			return EntData<T>(pEntity, offset, size);
		}

		template<EntType_c T>
		T GetEntDataEnt2(AutoEntity<CBaseEntity*> pEntity, unsigned short offset)
		{
			CBaseHandle ret = GetEntData<CBaseHandle>(pEntity, offset);
			return ent_cast<T>(ret);
		}

#if __cpp_lib_span
		template<class T>
		std::span<T, std::dynamic_extent> GetEntDataArray(AutoEntity<CBaseEntity*> pEntity, unsigned short offset, std::size_t arraysize, int size=sizeof(T)) {
			T *first = &EntData<T>(pEntity, offset, size);
			return std::span<T, std::dynamic_extent>(first, arraysize);
		}
#endif
		template<class T>
		T &SetEntData(AutoEntity<CBaseEntity*> pEntity, unsigned short offset, const T &value, int size=sizeof(T), bool bChangeState=false) {
			if(bChangeState)
			{
				edict_t *pEdict = cbase2edict(pEntity);
				gamehelpers->SetEdictStateChanged(pEdict, offset);
			}
			return EntData<T>(pEntity, offset, size) = value;
		}

		template<EntType_c T>
		void SetEntDataEnt2(AutoEntity<CBaseEntity*> pEntity, unsigned short offset, const T& other, bool bChangeState = false)
		{
			CBaseHandle& ent = EntData<CBaseHandle>(pEntity, offset);
			CBaseEntity* ent_set = ent_cast<CBaseEntity*>(other);
			IHandleEntity* ent_set2 = (IHandleEntity*)ent_set;
			ent.Set(ent_set2);
			if (bChangeState)
			{
				edict_t* pEdict = cbase2edict(pEntity);
				gamehelpers->SetEdictStateChanged(pEdict, offset);
			}
		}

		constexpr struct {} Prop_Data = {};
		constexpr struct {} Prop_Send = {};

		template<class T = cell_t>
		T &EntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t *td = info.prop;
			ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));

			return EntData<T>(pEntity, offset, size);
		}
		template<class T = cell_t>
		const T &GetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Data, prop, size, element);
		}
		template<class T = cell_t>
		T &SetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char *prop, const T &value, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Data, prop, size, element) = value;
		}
		inline std::size_t GetEntPropArraySize(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char *prop) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if(!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t *td = info.prop;
			return td->fieldSize;
		}
		template<EntType_c T = CBaseHandle>
		T GetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char* prop, int element = 0) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t* td = info.prop;
			ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));
			switch (td->fieldType)
			{
			case FIELD_EHANDLE:
				return ent_cast<T>(EntData<CBaseHandle>(pEntity, offset));
			case FIELD_CLASSPTR:
				return ent_cast<T>(EntData<CBaseEntity *>(pEntity, offset));
			case FIELD_EDICT:
				return ent_cast<T>(EntData<edict_t *>(pEntity, offset));
			case FIELD_CUSTOM:
				if ((td->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
					return ent_cast<T>(EntData<variant_t>(pEntity, offset).eVal);
			default:
				break;
			}
			throw std::runtime_error("Data field is not an entity nor edict");
		}
		template<EntType_c T = CBaseHandle>
		void SetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char* prop, const T& other, int element = 0) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t* td = info.prop;
			ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));
			switch (td->fieldType)
			{
			case FIELD_EHANDLE:
			{
				CBaseHandle& ent = EntData<CBaseHandle>(pEntity, offset);
				CBaseEntity* ent_set = ent_cast<CBaseEntity*>(other);
				IHandleEntity* ent_set2 = (IHandleEntity*)ent_set;
				return ent.Set(ent_set2), void();
			}
			case FIELD_CLASSPTR:
				return EntData<CBaseEntity*>(pEntity, offset) = ent_cast<CBaseEntity*>(other), void();
			case FIELD_EDICT:
				return EntData<edict_t*>(pEntity, offset) = ent_cast<edict_t*>(other), void();
			case FIELD_CUSTOM:
				if ((td->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
				{
					CBaseHandle& ent = EntData<variant_t>(pEntity, offset).eVal;
					CBaseEntity* ent_set = ent_cast<CBaseEntity*>(other);
					IHandleEntity* ent_set2 = (IHandleEntity*)ent_set;
					return ent.Set(ent_set2), void();
				}
			default:
				break;
			}
			throw std::runtime_error("Data field is not an entity nor edict");
		}

		template<class T = cell_t>
		T &EntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, int size = sizeof(T), int element=0) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable *pNet = ((IServerUnknown *)pEntity)->GetNetworkable();
			if(!pNet)
				throw std::runtime_error("Edict is not networkable");

			if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
				throw std::runtime_error("Property not found");

			SendProp *pProp = info.prop;
			ptrdiff_t offset = info.actual_offset;

			T *data = (T *)(reinterpret_cast<intptr_t>(static_cast<CBaseEntity*>(pEntity)) + offset);
			return *data;
		}
		template<class T = cell_t>
		const T &GetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Send, prop, size, element);
		}
		template<class T = cell_t>
		T &SetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, const T &value, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Send, prop, size, element) = value;
		}
		inline std::size_t GetEntPropArraySize(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable *pNet = ((IServerUnknown *)pEntity)->GetNetworkable();
			if(!pNet)
				throw std::runtime_error("Edict is not networkable");

			if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
				throw std::runtime_error("Property not found");

			if (info.prop->GetType() != DPT_DataTable)
			{
				return 0;
			}

			SendTable *pTable = info.prop->GetDataTable();
			if (!pTable)
			{
				throw std::runtime_error("Error looking up DataTable for prop");
			}

			return pTable->GetNumProps();
		}
		template<EntType_c T = CBaseHandle>
		T GetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char* prop, int element = 0) {
			CBaseHandle ent = GetEntProp<CBaseHandle>(pEntity, Prop_Send, prop, element);
			return ent_cast<T>(ent);
		}
		template<EntType_c T = CBaseHandle>
		void SetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char* prop, const T& other, int element = 0) {
			CBaseHandle &ent = EntProp<CBaseHandle>(pEntity, Prop_Send, prop, element);
			CBaseEntity* ent_set = ent_cast<CBaseEntity*>(other);
			IHandleEntity* ent_set2 = (IHandleEntity*)ent_set;
			ent.Set(ent_set2);
		}

#if __cpp_lib_span
		template<class T = cell_t, class PropType>
		std::span<T, std::dynamic_extent> GetEntPropArray(AutoEntity<CBaseEntity*> pEntity, PropType pt, const char* prop) {
			std::size_t arraysize = GetEntPropArraySize(pEntity, pt, prop);
			T* first = &EntProp<T>(pEntity, pt, prop);
			return std::span<T, std::dynamic_extent>(first, arraysize);
		}
#endif
	}
}