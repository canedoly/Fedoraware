#include "CritHack.h"
#define MASK_SIGNED 0x7FFFFFFF

int round_damage = 0;
int cached_damage = 0;

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

bool CCritHack::CanCrit(CBaseCombatWeapon* pWeapon)
{
	float CritChance = Utils::ATTRIB_HOOK_FLOAT(1, "mult_crit_chance", pWeapon, 0, 1);
	if (CritChance == 0)
	{
		return false;
	}

	//list of weapons that cant random crit, but dont have the attribute for it
	switch (pWeapon->GetWeaponID())
	{
		//scout
		case TF_WEAPON_JAR_MILK:
		//soldier
		case TF_WEAPON_BUFF_ITEM:
		//pyro
		case TF_WEAPON_JAR_GAS:
		case TF_WEAPON_FLAME_BALL:
		case TF_WEAPON_ROCKETPACK:
		//demo
		case TF_WEAPON_PARACHUTE: //also for soldier
		//heavy
		case TF_WEAPON_LUNCHBOX:
		//engineer
		case TF_WEAPON_PDA_ENGINEER_BUILD:
		case TF_WEAPON_PDA_ENGINEER_DESTROY:
		case TF_WEAPON_LASER_POINTER:
		//medic
		case TF_WEAPON_MEDIGUN:
		//sniper
		case TF_WEAPON_SNIPERRIFLE:
		case TF_WEAPON_SNIPERRIFLE_CLASSIC:
		case TF_WEAPON_SNIPERRIFLE_DECAP:
		case TF_WEAPON_COMPOUND_BOW:
		case TF_WEAPON_JAR:
		//spy
		case TF_WEAPON_KNIFE:
		case TF_WEAPON_PDA_SPY_BUILD:
		case TF_WEAPON_PDA_SPY:
			return false;
			break;
		default: return true; break;
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

float CCritHack::GetWithdrawMult(CBaseCombatWeapon* pWeapon)
{
	const auto count = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xA5C) + 1);
	const auto checks = static_cast<float>(*reinterpret_cast<int*>(pWeapon + 0xA58) + 1);

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
	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();
	auto bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	if (damage) {
		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat()) {
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}
	}

	if (GetWithdrawAmount(pWeapon) > bucket) { return false; }
	return true;
}

int CCritHack::GetShotsUntilCrit(CBaseCombatWeapon* pWeapon)
{
	const auto backupBucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	const auto backupAttempts = *reinterpret_cast<float*>(pWeapon + 0xA58);

	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();

	int shots;
	for (shots = 0; shots < ShotsToFill + 1; shots++)
	{
		if (CanWithdrawFromBucket(pWeapon, true)) { break; }

		auto bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
		auto attempts = *reinterpret_cast<float*>(pWeapon + 0xA58);

		if (bucket < tf_weapon_criticals_bucket_cap->GetFloat())
		{
			bucket += static_cast<float>(AddedPerShot);
			bucket = std::min(bucket, tf_weapon_criticals_bucket_cap->GetFloat());
		}

		attempts++;

		*reinterpret_cast<float*>(pWeapon + 0xA54) = bucket;
		*reinterpret_cast<float*>(pWeapon + 0xA58) = attempts;
	}

	*reinterpret_cast<float*>(pWeapon + 0xA54) = backupBucket;
	*reinterpret_cast<float*>(pWeapon + 0xA58) = backupAttempts;
	return shots;
}

int CCritHack::GetDamageUntilCrit(CBaseCombatWeapon* pWeapon)
{
	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return 0; }

	CTFPlayerResource* cResource = g_EntityCache.GetPR();

	auto [observed, needed] = GetCritMultInfo(pWeapon);
	if (observed <= needed || pWeapon->GetSlot() == 2) { return 0; }

	int round_damage = cResource->GetDamage(pLocal->GetIndex());	// round_damage
	int cached_damage = round_damage - G::MeleeDamage;	// cached_damage

	int rdamage = G::NormalDamage;
	int cdamage = rdamage - G::MeleeDamage;

	int damage = std::ceil(G::CritDamage * (2.0f * needed + 1.0f) / (3.0f * needed));
	//return damage - (round_damage - cached_damage);
	//return damage - (G::NormalDamage / 2);
	return damage - (round_damage - cached_damage);
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

	if (CritTicks.size() >= Vars::Test::CritTicks.Value)
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

	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();

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
			for (int tries = 0; tries < 10; tries++)
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

	const auto& pLocal = g_EntityCache.GetLocal();
	CTFPlayerResource* cResource = g_EntityCache.GetPR();

	int round_damage = cResource->GetDamage(pLocal->GetIndex());	// round_damage
	int cached_damage = round_damage - 0;	// cached_damage

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
	if (Vars::CritHack::Indicators.Value == 0) { return; }
	if (!IsEnabled() || !G::CurrentUserCmd) { return; }

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive()) { return; }

	const auto& pWeapon = pLocal->GetActiveWeapon();
	if (!pWeapon) { return; }

	const int x = Vars::CritHack::IndicatorPos.c;
	int currentY = Vars::CritHack::IndicatorPos.y;

	static auto tf_weapon_criticals_bucket_cap = g_ConVars.FindVar("tf_weapon_criticals_bucket_cap");
	const float bucketCap = tf_weapon_criticals_bucket_cap->GetFloat();

	const float bucket = *reinterpret_cast<float*>(pWeapon + 0xA54);
	const int seedRequests = *reinterpret_cast<int*>(pWeapon + 0xA5C);

	int longestW = 40;
	auto [observed, needed] = GetCritMultInfo(pWeapon);

	const int nClassNum = pLocal->GetClassNum();
	static int lastClass = 0;

	int w = 0, h = 0;


	if (Vars::CritHack::Indicators.Value == 1)
	{
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
			if (ShouldCrit())
			{
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 70, 190, 50, 255 }, ALIGN_CENTERHORIZONTAL, "Forcing crits...");
			}
			if (CritTicks.size() == 0 || observed > needed)
			{
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,0,0,255 }, ALIGN_CENTERHORIZONTAL, L"Crit Banned");

				const auto critText = tfm::format("%.3f < %.3f", observed, needed);
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());

			}
			const std::wstring bucketstr = L"Bucket: " + std::to_wstring(static_cast<int>(bucket)) + L"/" + std::to_wstring(static_cast<int>(bucketCap));
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 181, 181, 181, 255 }, ALIGN_CENTERHORIZONTAL, bucketstr.c_str());
			int w, h;
			I::VGuiSurface->GetTextSize(g_Draw.m_vecFonts.at(FONT_INDICATORS).dwFont, bucketstr.c_str(), w, h);
			if (w > longestW)
			{
				longestW = w;
			}
		}
	}
	if (Vars::CritHack::Indicators.Value == 2)
	{
		Color_t BarClr;
		
		int barX = x - 44;
		int barY = currentY + 15;
		int damage = GetDamageUntilCrit(pWeapon);
		if (damage > 8000)
		{
			damage = 8000;	// limit the damage to 8000
		}

		if (!AreRandomCritsEnabled())
		{
			BarClr = Vars::Menu::Colors::MenuAccent;
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, BarClr, ALIGN_CENTERHORIZONTAL, L"Server disabled crits");
		}
		if (!CanCrit() && AreRandomCritsEnabled())
		{
			BarClr = Vars::Menu::Colors::MenuAccent;
			g_Draw.String(FONT_INDICATORS, x, currentY += 15, BarClr, ALIGN_CENTERHORIZONTAL, L"This weapon can't randomly crit");
		}
		if (CanCrit() && AreRandomCritsEnabled())
		{
			if (observed > needed)
			{
				BarClr = { 255, 32, 32, 255 };
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, BarClr, ALIGN_CENTERHORIZONTAL, L"Crit Banned");
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 213, 213, 24, 255 }, ALIGN_CENTERHORIZONTAL, L"Deal %d Damage", damage);
				const auto critText = tfm::format("Observed %.3f > %.3f Needed", observed, needed);
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 213, 213, 24, 255 }, ALIGN_CENTERHORIZONTAL, critText.c_str());

			}
			else
			{
				BarClr = { 0,255,0,255 };
				g_Draw.String(FONT_INDICATORS, x, currentY += 15, BarClr, ALIGN_CENTERHORIZONTAL, L"Ready for crits");
			}
		}
	}

	if (Vars::Debug::DebugInfo.Value)
	{

		CTFPlayerResource* cResource = g_EntityCache.GetPR();

		auto [observed, needed] = GetCritMultInfo(pWeapon);

		int round_damage = cResource->GetDamage(pLocal->GetIndex());	// round_damage
		int cached_damage = round_damage - G::MeleeDamage;	// cached_damage

		int rdamage = G::NormalDamage;
		int cdamage = rdamage - G::MeleeDamage;

		int damage = std::ceil(G::CritDamage * (2.0f * needed + 1.0f) / (3.0f * needed));
		int result_damage = damage - (round_damage - cached_damage);
		int result_damage2 = damage - (rdamage - cdamage);

		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Damage: %d", damage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Result Damage: %d", result_damage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Result Damage2: %d", result_damage2);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"r Damage: %d", rdamage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"c Damage: %d", cdamage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"round Damage: %d", round_damage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"cached Damage: %d", cached_damage);

		g_Draw.String(FONT_INDICATORS, x, currentY += 30, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Normal Damage: %d", G::NormalDamage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Crit Damage: %d", G::CritDamage);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Melee Damage: %d", G::MeleeDamage);

		g_Draw.String(FONT_INDICATORS, x, currentY += 30, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Observed: %.3f", observed);
		g_Draw.String(FONT_INDICATORS, x, currentY += 15, { 255,255,255,255 }, ALIGN_CENTERHORIZONTAL, L"Needed: %.3f", needed);
	}
	IndicatorW = longestW * 2;
	IndicatorH = currentY;
	lastClass = nClassNum;
}

void CCritHack::FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash)
{
	if (!I::EngineClient->IsConnected() || !I::EngineClient->IsInGame())
	{
		return;
	}

	const auto& pLocal = g_EntityCache.GetLocal();

	if (const auto pLocal = g_EntityCache.GetLocal())
	{
		if (uNameHash == FNV1A::HashConst("teamplay_round_start") ||
			uNameHash == FNV1A::HashConst("client_disconnect") ||
			uNameHash == FNV1A::HashConst("client_beginconnect") ||
			uNameHash == FNV1A::HashConst("game_newmap"))
		{
			LastCritTick = -1;
			LastBucket = -1.f;

			ShotsUntilCrit = 0;
			AddedPerShot = 0;
			ShotsToFill = 0;
			TakenPerCrit = 0;

			int round_damage = 0;
			int cached_damage = 0;
			int rdamage = 0;
			int cdamage = 0;
			G::CritDamage = 0;
			G::MeleeDamage = 0;
			G::NormalDamage = 0;

		}
		if (uNameHash == FNV1A::HashConst("player_hurt"))
		{
			if (const auto pEntity = I::ClientEntityList->GetClientEntity(
				I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid"))))
			{
				const auto nAttacker = pEvent->GetInt("attacker");
				const auto nHealth = pEvent->GetInt("health");
				const auto nDamage = pEvent->GetInt("damageamount");
				const auto bCrit = pEvent->GetBool("crit");
				const int nIndex = pEntity->GetIndex();
				if (pEntity == pLocal) { return; }

				const auto& pWeapon = g_EntityCache.GetWeapon();
				bool isMelee = false;
				if (pWeapon->GetSlot() == 2)
				{
					isMelee = true;
				}

				PlayerInfo_t pi{};

				{
					I::EngineClient->GetPlayerInfo(I::EngineClient->GetLocalPlayer(), &pi);
					if (nAttacker != pi.userID) { return; }
				}

				I::EngineClient->GetPlayerInfo(nIndex, &pi);

				if (bCrit && !isMelee && CCritHack::CanCrit())
				{
					G::CritDamage += nDamage;
				}
				else if (isMelee)
				{
					G::MeleeDamage += nDamage;
				}
				else
				{
					G::NormalDamage += nDamage;
				}
			}
		}
	}
}

//void CCritHack::FireEvent(CGameEvent* pEvent, const FNV1A_t uNameHash)
//{
//	const auto& pLocal = g_EntityCache.GetLocal();
//	CTFPlayerResource* cResource = g_EntityCache.GetPR();
//	int n = 1;
//	for (n < I::EngineClient->GetMaxClients(); n++; )
//	{
//		CBaseEntity* pEntity = I::ClientEntityList->GetClientEntity(n);
//	}
//	CBaseEntity* pEntity = nullptr;
//	const auto& pWeapon = g_EntityCache.GetWeapon();
//
//	int round_damage = cResource->GetDamage(pLocal->GetIndex());	// round_damage
//	int cached_damage = round_damage - G::MeleeDamage;	// cached_damage
//
//
//	const int oHealth = cResource->GetHealth(n);
//
//	if (FNV1A::HashConst("teamplay_round_start") ||
//		FNV1A::HashConst("client_disconnect") ||
//		FNV1A::HashConst("client_beginconnect") ||
//		FNV1A::HashConst("game_newmap"))
//	{
//		// TODO: Clear CritCmds
//		LastCritTick = -1;
//		LastBucket = -1.f;
//
//		ShotsUntilCrit = 0;
//		AddedPerShot = 0;
//		ShotsToFill = 0;
//		TakenPerCrit = 0;
//
//		int total_damage = 0;	// round_damage
//		int round_damage = 0;	// cached_damage
//
//	}
//
//	if (FNV1A::HashConst("player_hurt"))
//	{
//		const auto pEntity = I::ClientEntityList->GetClientEntity(
//			I::EngineClient->GetPlayerForUserID(pEvent->GetInt("userid")));
//
//		int nHealth = pEvent->GetInt("health");
//		int nAttacker = pEvent->GetInt("attacker");
//		int nDamage = pEvent->GetInt("damageamount");
//		const bool bCrit = pEvent->GetBool("crit");
//
//
//		if (nAttacker == pLocal->GetIndex())
//		{
//			if (pEntity != pLocal)
//			{
//				bool isMelee = false;
//				if (pWeapon->GetSlot() == 2)
//				{
//					isMelee = true;
//				}
//
//				if (!isMelee)
//				{
//					if (bCrit)
//					{
//						G::CritDamage += nDamage;
//					}
//				}
//				else
//				{
//					G::MeleeDamage += nDamage;
//				}
//			}
//		}
//	}
//}
