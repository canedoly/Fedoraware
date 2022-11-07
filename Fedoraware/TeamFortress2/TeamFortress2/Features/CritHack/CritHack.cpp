#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

// i hate crithack
static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();

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
	if (!I::EngineClient->IsInGame()) { return false; }

	return true;
}

bool CCritHack::CanCrit()
{
	// ty stax for the idea
	switch (G::CurItemDefIndex)
	{

	// scout
	case Scout_s_CritaCola:
	case Scout_s_BonkAtomicPunch:
	case Scout_s_MadMilk:
	case Scout_s_MutatedMilk:
	case Scout_s_FestiveBonk:
	case Scout_s_TheFlyingGuillotineG:
	case Scout_s_TheFlyingGuillotine:

	// soldier
	case Soldier_m_TheCowMangler5000:
	case Soldier_t_TheMarketGardener:
	case Soldier_m_RocketJumper:
	case Soldier_s_TheBASEJumper:
	case Soldier_s_TheBuffBanner:
	case Soldier_s_TheBattalionsBackup:
	case Soldier_s_Gunboats:
	case Soldier_s_TheConcheror:
	case Soldier_s_TheMantreads:
	case Soldier_s_FestiveBuffBanner:
	case Soldier_t_TheHalfZatoichi:

	// pyro
	case Pyro_m_BarnBurner:
	case Pyro_m_ThePhlogistinator:
	case Pyro_m_DragonsFury:
	case Pyro_s_TheManmelter:
	case Pyro_s_ThermalThruster:
	case Pyro_s_GasPasser:
	case Pyro_t_TheAxtinguisher:
	case Pyro_t_NeonAnnihilator:
	case Pyro_t_NeonAnnihilatorG:

	// demoman
	case Demoman_m_AliBabasWeeBooties:
	case Demoman_m_TheBootlegger:
	case Demoman_s_TheCharginTarge:
	case Demoman_s_StickyJumper:
	case Demoman_s_TheSplendidScreen:
	case Demoman_s_TheTideTurner:
	case Demoman_s_FestiveTarge:
	case Demoman_t_TheEyelander:
	case Demoman_t_HorselessHeadlessHorsemannsHeadtaker:
	case Demoman_t_UllapoolCaber:
	case Demoman_t_TheClaidheamhMor:
	case Demoman_t_ThePersianPersuader:
	case Demoman_t_NessiesNineIron:
	case Demoman_t_FestiveEyelander:

	// heavy
	case Heavy_s_Sandvich:
	case Heavy_s_TheDalokohsBar:
	case Heavy_s_TheBuffaloSteakSandvich:
	case Heavy_s_Fishcake:
	case Heavy_s_RoboSandvich:
	case Heavy_s_FestiveSandvich:
	case Heavy_s_SecondBanana:

	// engineer
	case Engi_m_TheFrontierJustice:
	case Engi_s_TheShortCircuit:
	case Engi_s_FestiveWrangler:
	case Engi_t_TheGunslinger:
	case Engi_t_TheSouthernHospitality:
	case Engi_p_ConstructionPDA:
	case Engi_p_ConstructionPDAR:
	case Engi_p_DestructionPDA:
	case Engi_p_PDA:

	// medic
	case Medic_s_MediGun:
	case Medic_s_MediGunR:
	case Medic_s_TheKritzkrieg:
	case Medic_s_TheQuickFix:
	case Medic_s_FestiveMediGun:
	case Medic_s_SilverBotkillerMediGunMkI:
	case Medic_s_GoldBotkillerMediGunMkI:
	case Medic_s_RustBotkillerMediGunMkI:
	case Medic_s_BloodBotkillerMediGunMkI:
	case Medic_s_CarbonadoBotkillerMediGunMkI:
	case Medic_s_DiamondBotkillerMediGunMkI:
	case Medic_s_SilverBotkillerMediGunMkII:
	case Medic_s_GoldBotkillerMediGunMkII:
	case Medic_s_TheVaccinator:

	// sniper
	case Sniper_m_SniperRifle:
	case Sniper_m_SniperRifleR:
	case Sniper_m_TheHuntsman:
	case Sniper_m_TheSydneySleeper:
	case Sniper_m_TheBazaarBargain:
	case Sniper_m_TheMachina:
	case Sniper_m_FestiveSniperRifle:
	case Sniper_m_TheHitmansHeatmaker:
	case Sniper_m_SilverBotkillerSniperRifleMkI:
	case Sniper_m_GoldBotkillerSniperRifleMkI:
	case Sniper_m_TheAWPerHand:
	case Sniper_m_RustBotkillerSniperRifleMkI:
	case Sniper_m_BloodBotkillerSniperRifleMkI:
	case Sniper_m_CarbonadoBotkillerSniperRifleMkI:
	case Sniper_m_DiamondBotkillerSniperRifleMkI:
	case Sniper_m_SilverBotkillerSniperRifleMkII:
	case Sniper_m_GoldBotkillerSniperRifleMkII:
	case Sniper_m_FestiveHuntsman:
	case Sniper_m_TheFortifiedCompound:
	case Sniper_m_TheClassic:
	case Sniper_m_ShootingStar:
	case Sniper_s_TheRazorback:
	case Sniper_s_Jarate:
	case Sniper_s_DarwinsDangerShield:
	case Sniper_s_CozyCamper:
	case Sniper_s_FestiveJarate:
	case Sniper_t_TheBushwacka:

	// spy
	case Spy_m_TheAmbassador:
	case Spy_m_TheEnforcer:
	case Spy_m_TheDiamondback:
	case Spy_m_FestiveAmbassador:
	case Spy_s_Sapper:
	case Spy_s_SapperR:
	case Spy_s_TheRedTapeRecorder:
	case Spy_s_TheRedTapeRecorderG:
	case Spy_s_TheApSapG:
	case Spy_s_FestiveSapper:
	case Spy_s_TheSnackAttack:
	case Spy_t_Knife:
	case Spy_t_KnifeR:
	case Spy_t_YourEternalReward:
	case Spy_t_ConniversKunai:
	case Spy_t_TheBigEarner:
	case Spy_t_TheWangaPrick:
	case Spy_t_TheSharpDresser:
	case Spy_t_TheSpycicle:
	case Spy_t_FestiveKnife:
	case Spy_t_TheBlackRose:
	case Spy_t_SilverBotkillerKnifeMkI:
	case Spy_t_GoldBotkillerKnifeMkI:
	case Spy_t_RustBotkillerKnifeMkI:
	case Spy_t_BloodBotkillerKnifeMkI:
	case Spy_t_CarbonadoBotkillerKnifeMkI:
	case Spy_t_DiamondBotkillerKnifeMkI:
	case Spy_t_SilverBotkillerKnifeMkII:
	case Spy_t_GoldBotkillerKnifeMkII:
	case Spy_t_PrinnyMachete:
	case Spy_d_DisguiseKitPDA:
	case Spy_w_InvisWatch:
	case Spy_w_InvisWatchR:
	case Spy_w_TheDeadRinger:
	case Spy_w_TheCloakandDagger:
	case Spy_w_EnthusiastsTimepiece:
	case Spy_w_TheQuackenbirdt:

	return false;

	}
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
	static KeyHelper critKey{ &Vars::CritHack::CritKey.Value };
	if (critKey.Down()) { return true; }
	if (G::CurWeaponType == EWeaponType::MELEE && Vars::CritHack::AlwaysMelee.Value) { return true; }

	//Check if auto melee crit is enabled and player is using melee
	if (Vars::CritHack::AutoMeleeCrit.Value && G::CurWeaponType == EWeaponType::MELEE)
	{
		//Base melee damage
		int MeleeDamage = 65;

		//Could be missing wepaons or reskins
		switch (G::CurItemDefIndex)
		{
		case Scout_t_SunonaStick:
		{
			//The Sun on a Stick has a -25% melee damage stat
			MeleeDamage = 26;
			break;
		}
		case Scout_t_TheFanOWar:
		{
			//The Fan O'War has a -75% melee damage stat
			MeleeDamage = 9;
			break;
		}
		case Scout_t_TheWrapAssassin:
		{
			//The Wrap Assassin has a -65% melee damage stat
			MeleeDamage = 12;
			break;
		}
		case Soldier_t_TheDisciplinaryAction:
		case Engi_t_TheJag:
		{
			//The Disciplinary Action and The Jag have a -25% melee damage stat
			MeleeDamage = 49;
			break;
		}
		case Soldier_t_TheEqualizer:
		{
			//The Equalizer does more damage the lower the local player's health is
			break;
		}
		case Pyro_t_HotHand:
		{
			//The Hot Hand has a -20% melee damage stat
			MeleeDamage = 28;
			break;
		}
		case Pyro_t_SharpenedVolcanoFragment:
		case Medic_t_Amputator:
		{
			//The Sharpened Volcano Fragment and The Amputator have a -20% melee damage stat
			MeleeDamage = 52;
			break;
		}
		case Pyro_t_TheBackScratcher:
		{
			//The Back Scratcher has a +25% melee damage stat
			MeleeDamage = 81;
			break;
		}
		case Demoman_t_TheScotsmansSkullcutter:
		{
			//The Scotsmans Skullcutter has a +20% melee damage stat
			MeleeDamage = 78;
			break;
		}
		case Heavy_t_WarriorsSpirit:
		{
			//The Warriors Spirit has a +30% melee damage stat
			MeleeDamage = 85;
			break;
		}
		case Sniper_t_TheTribalmansShiv:
		{
			//The Tribalmans Shiv has a -50% melee damage stat
			MeleeDamage = 37;
			break;
		}
		case Sniper_t_TheShahanshah:
		{
			//The Shahanshah has a -25% melee damage stat when above half health and a +25% when below half health
			//81 if below half health
			//49 if above half health
			break;
		}
		default: break;
		}

		CBaseEntity* Player;
		if ((Player = I::ClientEntityList->GetClientEntity(G::CurrentTargetIdx)))
		{
			if (G::CurItemDefIndex == Heavy_t_TheHolidayPunch)
			{
				if (Player->OnSolid())
					return true;
			}

			if (MeleeDamage <= Player->GetHealth())
				return true;
		}
	}

	return false;
}

// crithack damage info lol
// todo: turn this into fware code
// static int damageUntilToCrit(IClientEntity *wep)
// {
//     // First check if we even need to deal damage at all
//     auto crit_info = critMultInfo(wep);
//     if (crit_info.first <= crit_info.second || g_pLocalPlayer->weapon_mode == weapon_melee)
//         return 0;

//     float target_chance = critMultInfo(wep).second;
//     // Formula taken from TotallyNotElite
//     int damage = std::ceil(crit_damage * (2.0f * target_chance + 1.0f) / (3.0f * target_chance));
//     return damage - (cached_damage - round_damage);
// }
//if (g_pPlayerResource->GetDamage(g_pLocalPlayer->entity_idx) < round_damage)
//round_damage = g_pPlayerResource->GetDamage(g_pLocalPlayer->entity_idx);
//cached_damage = g_pPlayerResource->GetDamage(g_pLocalPlayer->entity_idx) - melee_damage;

//int damage = event->GetInt("damageamount");
// imagine there's event stuff
// {
// 	if (damage_dealt)
// 	{
// 		if (event->GetBool("crit"))
//         crit_damage += damage;
// 	}
// }



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

// float CCritHack::GetWithdrawMult(CBaseCombatWeapon* pWeapon)
// {
// 	const auto count = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa5c) + 1);
// 	const auto checks = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xa58) + 1);

// 	float multiply = 0.5;
// 	if (pWeapon->GetSlot() != 2) { multiply = Math::RemapValClamped(count / checks, .1f, 1.f, 1.f, 3.f); }

// 	return multiply * 3.f;
// }

// float CCritHack::GetWithdrawAmount(CBaseCombatWeapon* pWeapon)
// {
// 	float amount = static_cast<float>(AddedPerShot) * GetWithdrawMult(pWeapon);
// 	if (pWeapon->IsRapidFire()) {
// 		amount = TakenPerCrit * GetWithdrawMult(pWeapon);
// 		reinterpret_cast<int&>(amount) &= ~1;
// 	}
// 	return amount;
// }

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

// bool CCritHack::CanWithdrawFromBucket(CBaseCombatWeapon* pWeapon, bool damage = true)
// {
// 	auto bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
// 	if (damage) {
// 		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat()) {
// 			bucket += static_cast<float>(AddedPerShot);
// 			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
// 		}
// 	}

// 	if (GetWithdrawAmount(pWeapon) > bucket) { return false; }
// 	return true;
// }

// int CCritHack::GetShotsUntilCrit(CBaseCombatWeapon* pWeapon)
// {
// 	// Backup weapon stats
// 	const auto backupBucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
// 	const auto backupAttempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

// 	int shots;
// 	for (shots = 0; shots < ShotsToFill + 1; shots++)
// 	{
// 		if (CanWithdrawFromBucket(pWeapon, true)) { break; }

// 		auto bucket = *reinterpret_cast<float*>(pWeapon + 0xa54);
// 		auto attempts = *reinterpret_cast<float*>(pWeapon + 0xa58);

// 		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat())
// 		{
// 			bucket += static_cast<float>(AddedPerShot);
// 			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
// 		}

// 		attempts++;

// 		*reinterpret_cast<float*>(pWeapon + 0xa54) = bucket;
// 		*reinterpret_cast<float*>(pWeapon + 0xa58) = attempts;
// 	}

// 	// Restore backup
// 	*reinterpret_cast<float*>(pWeapon + 0xa54) = backupBucket;
// 	*reinterpret_cast<float*>(pWeapon + 0xa58) = backupAttempts;
// 	return shots;
// }

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

	if (CritTicks.size() >= 512)
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
			for (int tries = 1; tries < 50; tries++)
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

	int longestW = 40;

	auto [observed, needed] = GetCritMultInfo(pWeapon);

	if (!AreRandomCritsEnabled())
	{
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, Vars::Menu::Colors::MenuAccent, ALIGN_CENTERHORIZONTAL, L"Server disabled crits");
	}

	if (!CanCrit() && AreRandomCritsEnabled())
	{
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, Vars::Menu::Colors::MenuAccent, ALIGN_CENTERHORIZONTAL, L"This weapon can't randomly crit");
	}

	if (CanCrit() && AreRandomCritsEnabled())
	{
		if (Vars::Debug::DebugInfo.Value)
		{
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255, 255, 255, 255, }, ALIGN_CENTERHORIZONTAL, tfm::format("%x", reinterpret_cast<float*>(pWeapon + 0xA54)).c_str());
		}
		// Are we currently forcing crits?
		if (ShouldCrit())
		{
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
		}
		//if (CritTicks.size() == 0 || observed > needed)
		if (CritTicks.size() == 0)
		{
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, Vars::Menu::Colors::MenuAccent, ALIGN_CENTERHORIZONTAL, L"Crit Banned");

			const auto critText = tfm::format("%.3f < %.3f", observed, needed);
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());
		}

		const std::wstring bucketstr = L"Bucket: " + std::to_wstring(static_cast<int>(bucket)) + L"/" + std::to_wstring(static_cast<int>(bucketCap));
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketstr.c_str());

		// const int withdrawAmount = GetWithdrawAmount(pWeapon);
		// const int potentialCrits = (bucket + AddedPerShot) / withdrawAmount;
		// const int maxCrits = bucket + AddedPerShot;
		// const auto critText = tfm::format("%s / %s Crits", potentialCrits, maxCrits);
		// g_Draw.String(FONT_MENU, g_ScreenSize.c, currentY += 15, Vars::Menu::Colors::MenuAccent, ALIGN_CENTERHORIZONTAL, critText.c_str());
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
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, seedText.c_str());
			I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, seedText.c_str(), w, h);
			if (w > longestW)
			{
				longestW = w;
			}
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, FoundCrits.c_str());
			I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, FoundCrits.c_str(), w, h);
			if (w > longestW)
			{
				longestW = w;
			}
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, commandNumber.c_str());
			I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, commandNumber.c_str(), w, h);
			if (w > longestW)
			{
				longestW = w;
			}
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
			//LastCritTick = -1;
			//LastBucket = -1.f;

			//ShotsUntilCrit = 0;
			AddedPerShot = 0;
			//ShotsToFill = 0;
			TakenPerCrit = 0;
			break;
		}
	}
}
