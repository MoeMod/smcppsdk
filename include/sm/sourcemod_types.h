#pragma once

#include <isaverestore.h>

class CEconItemView;
class CMultiInputVar;

class IPhysicsObject;
class IServerUnknown;
class IServerEntity;
class CBaseEntity;
class CGameRules;
class CBaseEntityOutput;

#ifndef BASEENTITY_H
class CBaseEntity : public IServerEntity {};
class CBaseAnimating : public CBaseEntity {};
class CBasePlayer : public CBaseAnimating {};
class CBaseCombatWeapon : public CBaseAnimating {};
#endif

namespace sm
{
	namespace datamap
	{
		template<fieldtype_t E, class T> struct DataMapIndex {
			using type = T;
			static constexpr fieldtype_t value = E;
		};
		template<class...Indexes> struct DataMapIndexList : Indexes... {};
		using DataMapIndexListInst = DataMapIndexList<
			DataMapIndex<FIELD_VOID, void>,
			DataMapIndex<FIELD_FLOAT, float>,			// Any floating point value
			DataMapIndex<FIELD_STRING, string_t>,			// A string ID (return from ALLOC_STRING)
			DataMapIndex<FIELD_VECTOR, Vector>,			// Any vector, QAngle, or AngularImpulse
			DataMapIndex<FIELD_QUATERNION, Quaternion>,		// A quaternion
			DataMapIndex<FIELD_INTEGER, int>,			// Any integer or enum
			DataMapIndex<FIELD_BOOLEAN, bool>,			// boolean, implemented as an int, I may use this as a hint for compression
			DataMapIndex<FIELD_SHORT, short>,			// 2 byte integer
			DataMapIndex<FIELD_CHARACTER, char>,		// a byte
			DataMapIndex<FIELD_COLOR32, Color>,			// 8-bit per channel r,g,b,a (32bit color)
			DataMapIndex<FIELD_EMBEDDED, void>,			// an embedded object with a datadesc, recursively traverse and embedded class/structure based on an additional typedescription
			DataMapIndex<FIELD_CUSTOM, void>,			// special type that contains function pointers to it's read/write/parse functions

			DataMapIndex<FIELD_CLASSPTR, CBaseEntity*>,			// CBaseEntity *
			DataMapIndex<FIELD_EHANDLE, EHANDLE>,			// Entity handle
			DataMapIndex<FIELD_EDICT, edict_t *>,			// edict_t *

			DataMapIndex<FIELD_POSITION_VECTOR, Vector>,	// A world coordinate (these are fixed up across level transitions automagically)
			DataMapIndex<FIELD_TIME, float>,				// a floating point time (these are fixed up automatically too!)
			DataMapIndex<FIELD_TICK, int>,				// an integer tick count( fixed up similarly to time)
			DataMapIndex<FIELD_MODELNAME, string_t>,		// Engine string that is a model name (needs precache)
			DataMapIndex<FIELD_SOUNDNAME, string_t>,		// Engine string that is a sound name (needs precache)

			DataMapIndex<FIELD_INPUT, CMultiInputVar>,			// a list of inputed data fields (all derived from CMultiInputVar)
			DataMapIndex<FIELD_FUNCTION, void>,			// A class function pointer (Think, Use, etc)

			DataMapIndex<FIELD_VMATRIX, VMatrix>,			// a vmatrix (output coords are NOT worldspace)

			// NOTE: Use float arrays for local transformations that don't need to be fixed up.
			DataMapIndex<FIELD_VMATRIX_WORLDSPACE, VMatrix>,// A VMatrix that maps some local space to world space (translation is fixed up on level transitions)
			DataMapIndex<FIELD_MATRIX3X4_WORLDSPACE, matrix3x4_t>,	// matrix3x4_t that maps some local space to world space (translation is fixed up on level transitions)

			DataMapIndex<FIELD_INTERVAL, interval_t>,			// a start and range floating point interval ( e.g., 3.2->3.6 == 3.2 and 0.4 )
			DataMapIndex<FIELD_MODELINDEX, int>,		// a model index
			DataMapIndex<FIELD_MATERIALINDEX, int>,	// a material index (using the material precache string table)

			DataMapIndex<FIELD_VECTOR2D, Vector2D>,			// 2 floats
			DataMapIndex<FIELD_INTEGER64, std::int64_t>,		// 64bit integer

			DataMapIndex<FIELD_VECTOR4D, Vector4D>			// 4 floats
		>;

		template<fieldtype_t E, class T>
		DataMapIndex<E, T> TypeIndexToType_impl(const DataMapIndex<E, T>&);
		template<class T, fieldtype_t E>
		DataMapIndex<E, T> TypeToTypeIndex_impl(const DataMapIndex<E, T>&);

		template<fieldtype_t E> struct TypeIndexToType : decltype(TypeIndexToType_impl<E>(DataMapIndexListInst())) {};
		template<class T> struct TypeToTypeIndex : decltype(TypeToTypeIndex_impl<T>(DataMapIndexListInst())) {};

		static_assert(TypeToTypeIndex<Vector4D>::value == FIELD_VECTOR4D);
		static_assert(std::is_same<typename TypeIndexToType<FIELD_VECTOR4D>::type, Vector4D>::value);

		template<class...Indexes>
		void IfTypeIndexImpl(fieldtype_t E, auto&& fn, DataMapIndexList<Indexes...>)
		{
			return ((Indexes::value == E ? fn(Indexes()) : void()), ...);
		}

		auto IfTypeIndex(fieldtype_t E, auto&& fn)
		{
			return IfTypeIndexImpl(E, std::forward<decltype(fn)>(fn), DataMapIndexListInst());
		}
	}
}