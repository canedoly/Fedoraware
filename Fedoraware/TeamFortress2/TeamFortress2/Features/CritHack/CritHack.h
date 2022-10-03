#pragma once
#include "../../SDK/SDK.h"

class CCritHack {
private:
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	bool ShouldCrit();
	//bool ShouldForceMelee(CBaseCombatWeapon* pWeapon);	//	compare distances between local & enemies, force crits if we are within swing range of enemy.
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	void ScanForCrits(const CUserCmd* pCmd, int loops = 10);
	int LastGoodCritTick(const CUserCmd* pCmd);
	float GetCritCap(CBaseCombatWeapon* pWeapon);
	std::pair<float, float> GetCritMultInfo(CBaseCombatWeapon* pWeapon);
	float GetWithdrawMult(CBaseCombatWeapon* pWeapon);
	float GetWithdrawAmount(CBaseCombatWeapon* pWeapon);
	bool CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage);
	bool CanWeaponCrit(CBaseCombatWeapon* pWeapon, bool draw);
	int GetShotsUntilCrit(CBaseCombatWeapon* pWeapon);
	//int DamageToNextCrit(CBaseCombatWeapon* pWeapon);	//	returns a positive value if we are crit banned

	std::vector<int> CritTicks{};

	//	TODO: Create & Restore to & from this struct when scanning for crits.
	//	Stop messing around with AddToBucket etc, just change values when scanning if needed.
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

	static inline ConVar* tf_weapon_criticals;
	static inline ConVar* tf_weapon_criticals_bucket_cap;

public:
	void Run(CUserCmd* pCmd);
	void Draw();
	void FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash);
	void Init();

	int IndicatorW;
	int IndicatorH;
	bool ProtectData = false;
};

ADD_FEATURE(CCritHack, CritHack)