#pragma once
#include "../../SDK/SDK.h"

class CCritHack
{
private:
	bool AreRandomCritsEnabled();
	bool IsEnabled();
	bool CanCrit();	// literally every weapon that can't crit, if its false then no crits for u
	bool ShouldCrit();
	//bool ShouldForceMelee(CBaseCombatWeapon* pWeapon);	//	compare distances between local & enemies, force crits if we are within swing range of enemy.
	bool IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon);
	float GetCritCap(CBaseCombatWeapon* pWeapon);
	std::pair<float, float> GetCritMultInfo(CBaseCombatWeapon* pWeapon);
	void ScanForCrits(const CUserCmd* pCmd, int loops = 10);
	int LastGoodCritTick(const CUserCmd* pCmd);
	//int DamageToNextCrit(CBaseCombatWeapon* pWeapon);	//	returns a positive value if we are crit banned
	// this would be very useful, but baan added this but no code in crithack.cpp 

	std::vector<int> CritTicks{};

	//	TODO: Create & Restore to & from this struct when scanning for crits.
	//	Stop messing around with AddToBucket etc, just change values when scanning if needed.
	struct stats_t
	{
		float flCritBucket;	// 0xA54
		int iNumAttacks;	// 0xA58
		int iNumCrits;		// 0xA5C
	};

public:
	void Run(CUserCmd* pCmd);
	void Draw();

	int IndicatorW;
	int IndicatorH;
	bool ProtectData = false;
};

ADD_FEATURE(CCritHack, CritHack)