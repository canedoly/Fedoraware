#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack

/* Returns whether random crits are enabled on the server */
bool CCritHack::AreRandomCritsEnabled()
{
	if (static auto tf_weapon_criticals = g_ConVars.FindVar("tf_weapon_criticals"); tf_weapon_criticals)
	{
		return tf_weapon_criticals->GetBool();
	}
	return true;
}

/* Returns whether the crithack should run */
bool CCritHack::IsEnabled()
{
	if (!Vars::CritHack::Active.Value) { return false; }
	if (!AreRandomCritsEnabled()) { return false; }
	if (!I::EngineClient->IsInGame()) { return false; }

	return true;
}

bool CCritHack::IsAttacking(const CUserCmd* pCmd, CBaseCombatWeapon* pWeapon)
{
	if (G::CurItemDefIndex == Soldier_m_TheBeggarsBazooka)
	{
		static bool bLoading = false;

		if (pWeapon->GetClip1() > 0)
		{
			bLoading = true;
		}

		if (!(pCmd->buttons & IN_ATTACK) && bLoading)
		{
			bLoading = false;
			return true;
		}
	}

	else
	{
		if (pWeapon->GetWeaponID() == TF_WEAPON_COMPOUND_BOW || pWeapon->GetWeaponID() == TF_WEAPON_PIPEBOMBLAUNCHER)
		{
			static bool bCharging = false;

			if (pWeapon->GetChargeBeginTime() > 0.0f)
			{
				bCharging = true;
			}

			if (!(pCmd->buttons & IN_ATTACK) && bCharging)
			{
				bCharging = false;
				return true;
			}
		}

		//pssst..
		//Dragon's Fury has a gauge (seen on the weapon model) maybe it would help for pSilent hmm..
		/*
		if (pWeapon->GetWeaponID() == 109) {
		}*/

		else
		{
			if ((pCmd->buttons & IN_ATTACK) && G::WeaponCanAttack)
			{
				return true;
			}
		}
	}

	return false;
}

bool CCritHack::ShouldCrit()
{
	static KeyHelper critKey{&Vars::CritHack::CritKey.Value};
	if (critKey.Down()) { return true; }
	if (G::CurWeaponType == EWeaponType::MELEE && Vars::CritHack::AlwaysMelee.Value) { return true; }

	return false;
}

float CCritHack::GetWithdrawMult(CBaseCombatWeapon* pWeapon)
{
	const auto count = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa5c) + 1);
	const auto checks = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa58) + 1);

	float multiply = 0.5;
	if (pWeapon->GetSlot() != 2) { multiply = Math::RemapValClamped(count / checks, .1f, 1.f, 1.f, 3.f); }

	return multiply * 3.f;
}

float CCritHack::GetWithdrawAmount(CBaseCombatWeapon* pWeapon)
{
	float amount = static_cast<float>(AddedPerShot) * GetWithdrawMult(pWeapon);
	if (pWeapon->IsRapidFire()) {
		amount = TakenPerCrit * GetWithdrawMult(pWeapon);
		reinterpret_cast<int&>(amount) &= ~1;
	}
	return amount;
}

float CCritHack::GetCritCap(CBaseCombatWeapon* pWeapon)
{
	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return 0.f; }

	const auto critMult = static_cast<float>(pLocal->GetCritMult());
	float chance = .02f;
	if (pWeapon->GetSlot() == 2)
	{
		chance = .15f;
	}

	float multChance = Utils::ATTRIB_HOOK_FLOAT(critMult * chance, "mult_crit_chance", pWeapon, nullptr, 1);
	if (pWeapon->IsRapidFire())
	{
		const float flTotalCritChance = std::clamp(0.02f * critMult, 0.01f, 0.99f);
		constexpr float flCritDuration = 2.0f;
		const float flNonCritDuration = (flCritDuration / flTotalCritChance) - flCritDuration;
		const float flStartCritChance = 1 / flNonCritDuration;
		multChance = Utils::ATTRIB_HOOK_FLOAT(flStartCritChance, "mult_crit_chance", pWeapon, nullptr, 1);
	}

	return multChance;
}

std::pair<float, float> CCritHack::GetCritMultInfo(CBaseCombatWeapon* pWeapon)
{
	float observed = pWeapon->ObservedCritChance();
	float needed = GetCritCap(pWeapon) + .1f;
	return { observed, needed };
}

bool CCritHack::CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage = true)
{
	auto bucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
	if (damage) {
		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat()) {
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}
	}

	if (GetWithdrawAmount(pWeapon) > bucket) { return false; }
	return true;
}

bool CCritHack::CanWeaponCrit(CBaseCombatWeapon* pWeapon, bool draw)
{
	if (!AreRandomCritsEnabled() || !AddedPerShot) { return false; }
	// if (!pWeapon->CanFireCriticalShot(false)) { return false; }
	if (GetCritCap(pWeapon) == 0.f) { return false; }
	if (!CanWithdrawFromBucket(pWeapon)) { return false; }
	if (!draw && !pWeapon->IsReadyToFire() && !pWeapon->IsRapidFire()) { return false; }

	auto [observed, needed] = GetCritMultInfo(pWeapon);
	if (observed > needed && pWeapon->GetSlot() != 2) { return false; }

	return true;
}

int CCritHack::GetShotsUntilCrit(CBaseCombatWeapon* pWeapon)
{
	// Backup weapon stats
	const auto backupBucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
	const auto backupAttempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

	int shots;
	for (shots = 0; shots < ShotsToFill + 1; shots++)
	{
		if (CanWithdrawFromBucket(pWeapon, true)) { break; }

		auto bucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
		auto attempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat())
		{
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}

		attempts++;

		*reinterpret_cast<float*>(pWeapon + 0xa54) = bucket;
		*reinterpret_cast<float*>(pWeapon + 0xa58) = attempts;
	}

	// Restore backup
	*reinterpret_cast<float*>(pWeapon + 0xa54) = backupBucket;
	*reinterpret_cast<float*>(pWeapon + 0xa58) = backupAttempts;
	return shots;
}

int CCritHack::LastGoodCritTick(const CUserCmd* pCmd)
{
	int retVal = -1;
	bool popBack = false;

	for (const auto& tick : CritTicks)
	{
		if (tick >= pCmd->command_number)
		{
			retVal = tick;
		}
		else
		{
			popBack = true;
		}
	}

	if (popBack)
	{
		CritTicks.pop_back();
	}

	return retVal;
}

void CCritHack::ScanForCrits(const CUserCmd* pCmd, int loops)
{
	static int previousWeapon = 0;
	static int previousCrit = 0;
	static int startingNum = pCmd->command_number;

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	if (G::IsAttacking || IsAttacking(pCmd, pWeapon)/* || pCmd->buttons & IN_ATTACK*/)
	{
		return;
	}

	const bool bRescanRequired = previousWeapon != pWeapon->GetIndex();
	if (bRescanRequired)
	{
		startingNum = pCmd->command_number;
		previousWeapon = pWeapon->GetIndex();
		CritTicks.clear();
	}

	if (CritTicks.size() >= 256)
	{
		return;
	}

	//CritBucketBP = *reinterpret_cast<float*>(pWeapon + 0xA54);
	ProtectData = true; //	stop shit that interferes with our crit bucket because it will BREAK it
	const int seedBackup = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
	for (int i = 0; i < loops; i++)
	{
		const int cmdNum = startingNum + i;
		*I::RandomSeed = MD5_PseudoRandom(cmdNum) & MASK_SIGNED;
		if (pWeapon->WillCrit())
		{
			CritTicks.push_back(cmdNum); //	store our wish command number for later reference
		}
	}
	startingNum += loops;
	ProtectData = false; //	we no longer need to be protecting important crit data

	//*reinterpret_cast<float*>(pWeapon + 0xA54) = CritBucketBP;
	*reinterpret_cast<int*>(pWeapon + 0xA5C) = 0; //	dont comment this out, makes sure our crit mult stays as low as possible
	//	crit mult can reach a maximum value of 3!! which means we expend 3 crits WORTH from our bucket
	//	by forcing crit mult to be its minimum value of 1, we can crit more without directly fucking our bucket
	//	yes ProtectData stops this value from changing artificially, but it still changes when you fire and this is worth it imo.

	*I::RandomSeed = seedBackup;
}

void CCritHack::Run(CUserCmd* pCmd)
{
	if (!IsEnabled()) { return; }

	const auto& pWeapon = g_EntityCache.GetWeapon();
	if (!pWeapon || !pWeapon->CanFireCriticalShot(false)) { return; }

	ScanForCrits(pCmd, 50); //	fill our vector slowly.

	const int closestGoodTick = LastGoodCritTick(pCmd); //	retrieve our wish
	if (IsAttacking(pCmd, pWeapon)) //	is it valid & should we even use it
	{
		if (ShouldCrit())
		{
			if (closestGoodTick < 0) { return; }
			pCmd->command_number = closestGoodTick; //	set our cmdnumber to our wish
			pCmd->random_seed = MD5_PseudoRandom(closestGoodTick) & MASK_SIGNED; //	trash poopy whatever who cares
		}
		else if (Vars::CritHack::AvoidRandom.Value) //	we don't want to crit
		{
			for (int tries = 1; tries < 25; tries++)
			{
				if (std::find(CritTicks.begin(), CritTicks.end(), pCmd->command_number + tries) != CritTicks.end())
				{
					continue; //	what a useless attempt
				}
				pCmd->command_number += tries;
				pCmd->random_seed = MD5_PseudoRandom(pCmd->command_number) & MASK_SIGNED;
				break; //	we found a seed that we can use to avoid a crit and have skipped to it, woohoo
			}
		}
	}


	// Update stats
	static int previousWeapon = 0;
	if (AddedPerShot == 0 || previousWeapon != pWeapon->GetIndex())
	{
		const auto& weaponData = pWeapon->GetWeaponData();
		const auto cap = tf_weapon_criticals_bucket_cap->GetFloat();
		int projectilesPerShot = weaponData.m_nBulletsPerShot;
		if (projectilesPerShot >= 1)
		{
			projectilesPerShot = Utils::ATTRIB_HOOK_FLOAT(projectilesPerShot, "mult_bullets_per_shot", pWeapon, nullptr, true);
		}
		else
		{
			projectilesPerShot = 1;
		}

		AddedPerShot = weaponData.m_nDamage;
		AddedPerShot = static_cast<int>(Utils::ATTRIB_HOOK_FLOAT(static_cast<float>(AddedPerShot), "mult_dmg", pWeapon, nullptr, true));
		AddedPerShot *= std::max(1, projectilesPerShot);
		ShotsToFill = static_cast<int>(cap / static_cast<float>(AddedPerShot));

		if (pWeapon->IsRapidFire())
		{
			TakenPerCrit = AddedPerShot;
			TakenPerCrit *= static_cast<int>(2.f / weaponData.m_flTimeFireDelay);
			if (TakenPerCrit * 3 > static_cast<int>(cap))
			{
				TakenPerCrit = static_cast<int>(cap / 3.f);
			}
		}
	}
	previousWeapon = pWeapon->GetIndex();
}

void CCritHack::Draw()
{
	if (!Vars::CritHack::Indicators.Value) { return; }
	if (!IsEnabled() || !G::CurrentUserCmd) { return; }

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	const int x = Vars::CritHack::IndicatorPos.c;
	int currentY = Vars::CritHack::IndicatorPos.y;

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	const int seedRequests = *reinterpret_cast<int*>(pWeapon + 0xA5C);
	const bool canCrit = CanWeaponCrit(pWeapon, true);
	if (!canCrit) { ShotsUntilCrit = GetShotsUntilCrit(pWeapon); }

	int longestW = 40;

	if (Vars::Debug::DebugInfo.Value)
	{
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {255, 255, 255, 255,}, ALIGN_CENTERHORIZONTAL, tfm::format("%x", reinterpret_cast<float*>(pWeapon + 0xA54)).c_str());
	}
	// Are we currently forcing crits?
	if (ShouldCrit())
	{
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {70, 190, 50, 255}, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
	}
	if (CritTicks.size() == 0)
	{
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {255,0,0,255}, ALIGN_CENTERHORIZONTAL, L"Crit Banned");
	}
	if (!canCrit)
	{
		auto [observed, needed] = GetCritMultInfo(pWeapon);
		const auto critText = tfm::format("%.3f < %.3f", observed, needed);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 248, 255, 51, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());
	}

	const int withdrawAmount = GetWithdrawAmount(pWeapon);
	const int potentialCrits = (bucket + AddedPerShot) / withdrawAmount;
	const auto critText = tfm::format("%s crits", potentialCrits);
	g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 14, 204, 0, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());
	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();
	const std::wstring bucketstr = L"Bucket: " + std::to_wstring(static_cast<int>(bucket)) + L"/" + std::to_wstring(static_cast<int>(bucketCap));
	g_Draw.String(FONT_INDICATORS, x, currentY += 15, {181, 181, 181, 255}, ALIGN_CENTERHORIZONTAL, bucketstr.c_str());
	int w, h;
	I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, bucketstr.c_str(), w, h);
	if (w > longestW)
	{
		longestW = w;
	}
	if (Vars::Debug::DebugInfo.Value)
	{
		const std::wstring seedText = L"m_nCritSeedRequests: " + std::to_wstring(seedRequests);
		const std::wstring FoundCrits = L"Found Crit Ticks: " + std::to_wstring(CritTicks.size());
		const std::wstring commandNumber = L"cmdNumber: " + std::to_wstring(G::CurrentUserCmd->command_number);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {181, 181, 181, 255}, ALIGN_CENTERHORIZONTAL, seedText.c_str());
		I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, seedText.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {181, 181, 181, 255}, ALIGN_CENTERHORIZONTAL, FoundCrits.c_str());
		I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, FoundCrits.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, {181, 181, 181, 255}, ALIGN_CENTERHORIZONTAL, commandNumber.c_str());
		I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, commandNumber.c_str(), w, h);
		if (w > longestW)
		{
			longestW = w;
		}
	}
	IndicatorW = longestW * 2;
	IndicatorH = currentY;
}

void CCritHack::FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	switch (uNameHash)
	{
		case FNV1A::HashConst("player_hurt"):
		{
			// TODO: This
			break;
		}

		case FNV1A::HashConst("teamplay_round_start"):
		case FNV1A::HashConst("client_disconnect"):
		case FNV1A::HashConst("client_beginconnect"):
		case FNV1A::HashConst("game_newmap"):
		{
			// TODO: Clear CritCmds
			LastCritTick = -1;
			LastBucket = -1.f;

			ShotsUntilCrit = 0;
			AddedPerShot = 0;
			ShotsToFill = 0;
			TakenPerCrit = 0;
			break;
		}
	}
}

void CCritHack::Init()
{
	tf_weapon_criticals = g_ConVars.FindVar("tf_weapon_criticals")
	tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap")
}
