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
		//native bool GetEntityNetClass(int edict, char[] clsname, int maxlength);
		inline const char* GetEntityNetClass(edict_t* edict) {
			CBaseEntity* pEntity = sm::ent_cast<CBaseEntity*>(edict);
			IServerUnknown* unk = (IServerUnknown*)pEntity;
			if (!pEntity) throw std::runtime_error("Your edict called is invalid.");
			IServerNetworkable* net = unk->GetNetworkable();
			if (!net) throw std::runtime_error("This edict what you have called is not networkable.");
			ServerClass* serverclass = net->GetServerClass();
			return serverclass->GetName();
		}
		inline int GetEntSendPropOffs(CBaseEntity* entity, const char* prop, bool actual = false)
		{
			std::string temp = GetEntityNetClass(sm::ent_cast<edict_t*>(entity));
			if (!temp.size()) return -1;
			sm_sendprop_info_t info = {};

			int local = -1;
			int offset = 0;
			if (!gamehelpers->FindSendPropInfo(temp.c_str(), prop, &info))
				throw std::runtime_error("Unable to find the send prop: " + (std::string() + temp + "::" + prop));
			local = info.prop->GetOffset();
			offset = info.actual_offset;

			return actual ? offset : local;
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
			{
				throw std::runtime_error(std::string() + "Prop not found: " + prop);
			}
			typedescription_t* td = info.prop;
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

			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
			{
				throw std::runtime_error(std::string() + "Prop not found: " + prop);
			}
			typedescription_t* td = info.prop;
			return td->fieldSize;
		}
		template<EntType_c T = CBaseHandle>
		T GetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char* prop, int element = 0) {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};

			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
			{
				throw std::runtime_error(std::string() + "Prop not found: " + prop);
			}
			typedescription_t* td = info.prop;
			ptrdiff_t offset = info.actual_offset + (element * (td->fieldSizeInBytes / td->fieldSize));

			switch (td->fieldType)
			{
			case FIELD_EHANDLE:
				return ent_cast<T>(EntData<CBaseHandle>(pEntity, offset));
			case FIELD_CLASSPTR:
				return ent_cast<T>(EntData<CBaseEntity*>(pEntity, offset));
			case FIELD_EDICT:
				return ent_cast<T>(EntData<edict_t*>(pEntity, offset));
			case FIELD_CUSTOM:
				if ((td->flags & FTYPEDESC_OUTPUT) == FTYPEDESC_OUTPUT)
					return ent_cast<T>(EntData<variant_t>(pEntity, offset).eVal);
			default:
				break;
			}
			throw std::runtime_error("Data field is not an entity or not an edict.");
		}
		template<EntType_c T = CBaseHandle>
		void SetEntPropEnt(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Data), const char* prop, const T& other, int element = 0) noexcept {
			assert(pEntity != nullptr);
			sm_datatable_info_t info = {};

			if (!gamehelpers->FindDataMapInfo(gamehelpers->GetDataMap(pEntity), prop, &info))
			{
				throw std::runtime_error(std::string() + "Prop not found: " + prop);
			}
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

		// For Prop_Send::String, please use Get/SetEntPropString, rather this function.
		template<class T = cell_t>
		T &EntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, int size = sizeof(T), int element=0) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable* pNet = ((IServerUnknown*)pEntity)->GetNetworkable();
			if (!pNet) throw std::runtime_error("Edict is not networkable");
			if (!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info)) throw std::runtime_error(std::string() + "Prop not found: " + prop);
			SendProp* pProp = info.prop;
			ptrdiff_t offset = info.actual_offset;
			T* data = (T*)(reinterpret_cast<intptr_t>(static_cast<CBaseEntity*>(pEntity)) + offset);
			return *data;
		}
		template<class T = cell_t>
		const T &GetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, int size=sizeof(T), int element=0) {
			return EntProp<T>(pEntity, Prop_Send, prop, size, element);
		}
		// Further investigation is required: Set is OK, but Get crashes.
		template<class T = cell_t>
		T &SetEntProp(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop, const T &value, int size=sizeof(T), int element=0) {
			if (std::is_same<T, const char*>::value || std::is_same<T, char*>::value) {
				const char* sValue = g_pSM->GetCoreConfigValue("FollowCSGOServerGuidelines");
				if (sValue && !strcasecmp(sValue, "no")) {
					throw std::runtime_error("You must set \"FollowCSGOServerGuidelines\" to process this function.");
				}
			}
			return EntProp<T>(pEntity, Prop_Send, prop, size, element) = value;
		}
		// I think that perhaps no goes with GetProxyFn to get Prop_Send string so that crashes the game.
		const char* GetEntPropString(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char* prop, int element=0) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable* pNet = ((IServerUnknown*)pEntity)->GetNetworkable();
			if (!pNet) throw std::runtime_error("Edict is not networkable");
			if (!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info)) throw std::runtime_error(std::string() + "Prop not found: " + prop);
			SendProp* pProp = info.prop;
			ptrdiff_t offset = info.actual_offset;
			const char* src;
			if (pProp->GetProxyFn()) {
				DVariant var;
				pProp->GetProxyFn()(pProp, pEntity, (const void*)(reinterpret_cast<intptr_t>(static_cast<CBaseEntity*>(pEntity)) + offset), &var, element, sm::ent_cast<cell_t>(pEntity));
				src = var.m_pString;
			} else {
				src = *(char**)(reinterpret_cast<intptr_t*>(static_cast<CBaseEntity*>(pEntity)) + offset);
			}
			return src;
		}
		// identification: FollowCSGOGuidelines, especially in SE_CSGO.
		inline std::size_t GetEntPropArraySize(AutoEntity<CBaseEntity*> pEntity, decltype(Prop_Send), const char *prop) {
			assert(pEntity != nullptr);
			sm_sendprop_info_t info = {};
			IServerNetworkable* pNet = ((IServerUnknown*)pEntity)->GetNetworkable();

			if (!pNet)
				throw std::runtime_error("Edict is not networkable");

			if (!gamehelpers->FindSendPropInfo(pNet->GetServerClass()->GetName(), prop, &info))
				throw std::runtime_error(std::string() + "Prop not found: " + prop);

			if (info.prop->GetType() != DPT_DataTable)
			{
				return 0;
			}

			SendTable* pTable = info.prop->GetDataTable();
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

		/**
		 * Gets an entity's render mode.
		 *
		 * @param entity        Entity index.
		 * @return              RenderMode value.
		 * @error               Invalid entity index, or lack of mod compliance.
		 */
		inline RenderMode_t GetEntityRenderMode(CBaseEntity* entity)
		{
			const char* prop = g_pGameConf->GetKeyValue("m_nRenderMode");

			if (!prop) {
				std::string buffer("m_nRenderMode");
				prop = buffer.c_str();
			}

			return sm::GetEntProp<RenderMode_t>(entity, sm::Prop_Send, prop, 1);
		}

		/**
		 * Sets an entity's render mode.
		 *
		 * @param entity        Entity index.
		 * @param mode          RenderMode value.
		 * @error               Invalid entity index, or lack of mod compliance.
		 */
		inline void SetEntityRenderMode(CBaseEntity* entity, RenderMode_t mode)
		{
			const char* prop = g_pGameConf->GetKeyValue("m_nRenderMode");

			if (!prop) {
				std::string buffer("m_nRenderMode");
				prop = buffer.c_str();
			}

			sm::SetEntProp<RenderMode_t>(entity, sm::Prop_Send, prop, mode, 1);
		}
	}
}