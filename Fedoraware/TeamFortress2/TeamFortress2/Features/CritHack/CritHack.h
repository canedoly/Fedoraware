#pragma once
#include "../../SDK/SDK.h"

class CCritHack
{
private:

	//struct player_status
	//{
	//	int health{};
	//	int clazz{};	// int for classes 5 being heavy, or 2 being soldier
	//	bool just_updated{};
	//};

	bool AreRandomCritsEnabled();
	bool IsEnabled();
	bool ShouldCrit();
	//bool ShouldForceMelee(CBaseCombatWeapon* pWeapon);	//	compare distances between local & enemies, force crits if we are within swing range of enemy.
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	float GetCritCap(CBaseCombatWeapon* pWeapon);
	std::pair<float, float> GetCritMultInfo(CBaseCombatWeapon* pWeapon);
	float GetWithdrawMult(CBaseCombatWeapon* pWeapon);
	float GetWithdrawAmount(CBaseCombatWeapon* pWeapon);
	bool CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage);
	int GetDamageUntilCrit(CBaseCombatWeapon* pWeapon);
	int GetShotsUntilCrit(CBaseCombatWeapon* pWeapon);
	void ScanForCrits(const CUserCmd* pCmd, int loops = Vars::Test::CritTicks.Value);
	int LastGoodCritTick(const CUserCmd* pCmd);

	std::vector<int> CritTicks{};

	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

	float LastBucket = -1.f;
	int LastCritTick = -1;
	int LastWeapon = 0;

	int ShotsUntilCrit = 0;
	int AddedPerShot = 0;
	int ShotsToFill = 0;
	int TakenPerCrit = 0;

	int round_damage = 0;
	int cached_damage = 0;
	int rdamage = 0;
	int cdamage = 0;

public:
	bool CanCrit(CBaseCombatWeapon* pWeapon);
	void Run(CUserCmd* pCmd);
	void Draw();
	void FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash);

	int IndicatorW;
	int IndicatorH;
	bool ProtectData = false;
};

ADD_FEATURE(CCritHack, CritHack)