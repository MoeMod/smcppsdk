#pragma once

#include <server_class.h>
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
		T &EntData(CBaseEntity *pEntity, unsigned short offset, int size=sizeof(int))
		{
			assert(pEntity != nullptr);
			T *data = (T *)((intptr_t)pEntity + offset);
			return *data;
		}
		template<class T>
		T &GetEntData(CBaseEntity *pEntity, unsigned short offset, int size=sizeof(int)) {
			return EntData<T>(pEntity, offset, size);
		}

#if __cpp_lib_span
		template<class T>
		std::span<T, std::dynamic_extent> GetEntDataArray(CBaseEntity *pEntity, unsigned short offset, std::size_t arraysize, int size=sizeof(int)) {
			T *first = &EntData<T>(pEntity, offset, size);
			return std::span<T, std::dynamic_extent>(first, arraysize);
		}
#endif
		template<class T>
		T &SetEntData(CBaseEntity *pEntity, unsigned short offset, const T &value, int size=sizeof(int), bool bChangeState=false) {
			if(bChangeState)
			{
				edict_t *pEdict = cbase2edict(pEntity);
				gamehelpers->SetEdictStateChanged(pEdict, offset);
			}
			return EntData<T>(pEntity, offset, size) = value;
		}

		constexpr struct {} Prop_Data = {};
		constexpr struct {} Prop_Send = {};

		template<class T = cell_t>
		T &EntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t *td = info.prop;
			ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));

			return EntData<T>(pEntity, offset, size);
		}
		template<class T = cell_t>
		const T &GetEntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Data, prop, size, element);
		}
		template<class T = cell_t>
		T &SetEntProp(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop, const T &value, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Data, prop, size, element) = value;
		}
		inline std::size_t GetEntPropArraySize(CBaseEntity *pEntity, decltype(Prop_Data), const char *prop) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};
			if(!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
				throw std::runtime_error("Property not found");
			typedescription_t *td = info.prop;
			return td->fieldSize;
		}

		template<class T = cell_t>
		T &EntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, int size = sizeof(T), int element=0) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable *pNet = ((IServerUnknown *)pEntity)->GetNetworkable();
			if(!pNet)
				throw std::runtime_error("Edict is not networkable");

			if(!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
				throw std::runtime_error("Property not found");

			SendProp *pProp = info.prop;
			ptrdiff_t offset = info.actual_offset;

			T *data = (T *)((intptr_t)pEntity + offset);
			return *data;
		}
		template<class T = cell_t>
		const T &GetEntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Send, prop, size, element);
		}
		template<class T = cell_t>
		T &SetEntProp(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop, const T &value, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Send, prop, size, element) = value;
		}
		inline std::size_t GetEntPropArraySize(CBaseEntity *pEntity, decltype(Prop_Send), const char *prop) {
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

#if __cpp_lib_span
		template<class T = cell_t, class PropType>
		std::span<T, std::dynamic_extent> GetEntPropArray(CBaseEntity* pEntity, PropType pt, const char* prop) {
			std::size_t arraysize = GetEntPropArraySize(pEntity, pt, prop);
			T* first = &EntProp<T>(pEntity, pt, prop);
			return std::span<T, std::dynamic_extent>(first, arraysize);
		}
#endif
	}
}