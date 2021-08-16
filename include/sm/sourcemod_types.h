#pragma once

class CEconItemView;

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
