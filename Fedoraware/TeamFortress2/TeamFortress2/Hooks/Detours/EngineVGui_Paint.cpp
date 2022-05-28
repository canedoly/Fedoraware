#include "../Hooks.h"

#include "../../SDK/Includes/icons.h"
#include "../../Features/SpyWarning/SpyWarning.h"
#include "../../Features/PlayerArrows/PlayerArrows.h"
#include "../../Features/ESP/ESP.h"
#include "../../Features/Misc/Misc.h"
#include "../../Features/Visuals/Visuals.h"
#include "../../Features/PlayerResource/PlayerResource.h"
#include "../../Features/CritHack/CritHack.h"
#include "../../Features/Menu/Menu.h"
#include "../../Features/Menu/SpectatorList/SpectatorList.h"
#include "../../Features/Menu/DTBar/DTBar.h"
#include "../../Features/Radar/Radar.h"
#include "../../Features/Followbot/Followbot.h"

MAKE_HOOK(EngineVGui_Paint, Utils::GetVFuncPtr(I::EngineVGui, 13), void, __fastcall,
		  void* ecx, void* edx, int iMode)
{
	static auto StartDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 64 A1 ? ? ? ? 6A FF 68 ? ? ? ? 50 64 89 25 ? ? ? ? 83 EC 14")));
	static auto FinishDrawing = reinterpret_cast<void(__thiscall*)(void*)>(g_Pattern.Find(
		_(L"vguimatsurface.dll"), _(L"55 8B EC 6A FF 68 ? ? ? ? 64 A1 ? ? ? ? 50 64 89 25 ? ? ? ? 51 56 6A 00")));

	if (!g_ScreenSize.w || !g_ScreenSize.h)
	{
		g_ScreenSize.Update();
	}

	//HACK: for some reason we need to do this
	{
		static bool bInitIcons = false;

		if (!bInitIcons)
		{
			for (int nIndex = 0; nIndex < ICONS::TEXTURE_AMOUNT; nIndex++)
			{
				ICONS::ID[nIndex] = -1;
				g_Draw.Texture(-200, 0, 18, 18, Colors::White, nIndex);
			}

			bInitIcons = true;
		}
	}

	Hook.Original<FN>()(ecx, edx, iMode);

	if (iMode & PAINT_UIPANELS)
	{
		//Update W2S
		{
			CViewSetup viewSetup = {};

			if (I::Client->GetPlayerView(viewSetup))
			{
				VMatrix worldToView = {}, viewToProjection = {}, worldToPixels = {};
				I::RenderView->GetMatricesForView(viewSetup, &worldToView, &viewToProjection,
															&g_GlobalInfo.m_WorldToProjection, &worldToPixels);
			}
		}

		StartDrawing(I::Surface);
		{
			auto OtherDraws = [&]() -> void
			{
				if (I::EngineVGui->IsGameUIVisible())
				{
					return;
				}

				//Proj aim line
				//This could use alot of improvement, but still subjectively better than a flying rec
				//Credits to JAGNEmk aka me x)

				if (Vars::Aimbot::Projectile::MovementSimulation.m_Var && !g_GlobalInfo.m_vPredictedPos.IsZero())
				{
					if (Vars::Visuals::MoveSimLine.m_Var)
					{
						for (size_t i = 0; i < g_GlobalInfo.predFutureLines.size(); i++)
						{
							Vec3 vScreenpast, vScreenfuture;
							if (Utils::W2S(g_GlobalInfo.predBeforeLines.at(i), vScreenpast))
							{
								if (Utils::W2S(g_GlobalInfo.predFutureLines.at(i), vScreenfuture))
								{
									g_Draw.Line(vScreenpast.x, vScreenpast.y, vScreenfuture.x, vScreenfuture.y,
												{ Vars::Aimbot::Projectile::PredictionColor });
								}
							}
						}
					}
				}
				else
				{
					if (!g_GlobalInfo.m_vPredictedPos.IsZero())
					{
						if (Vars::Visuals::AimPosSquare.m_Var)
						{
							Vec3 vProjAimStart, vProjAimEnd = Vec3(g_ScreenSize.c, g_ScreenSize.h, 0.0f);
							if (Utils::W2S(g_GlobalInfo.linearPredLine, vProjAimStart) && Utils::W2S(
								g_GlobalInfo.m_vPredictedPos, vProjAimEnd))
							{
								g_Draw.Line(
									vProjAimStart.x,
									vProjAimStart.y,
									vProjAimEnd.x,
									vProjAimEnd.y,
									{ 255, 255, 255, 255 } //Set this to a var if u wantto idc
								);
							}
						}
					}
				}

				//Tickbase info
				if (Vars::Misc::CL_Move::Enabled.m_Var)
				{
					const auto& pLocal = g_EntityCache.m_pLocal;
					const auto& pWeapon = g_EntityCache.m_pLocalWeapon;

					if (pLocal && pWeapon)
					{
						if (pLocal->GetLifeState() == LIFE_ALIVE)
						{
							const int nY = (g_ScreenSize.h / 2) + 20;

							static Color_t color1, color2;

							if (g_GlobalInfo.m_nWaitForShift)
							{
								color1 = Colors::DTBarIndicatorsCharging.startColour;
								color2 = Colors::DTBarIndicatorsCharging.endColour; 
							}
							else
							{
								color1 = Colors::DTBarIndicatorsCharged.startColour;
								color2 = Colors::DTBarIndicatorsCharged.endColour;
							}

							// Rijin V1 DT Bar
							if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 1)
							{
								const auto maxWidth = static_cast<float>(Vars::Misc::CL_Move::DTTicks.m_Var * Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var);
								const float dtOffset = g_ScreenSize.c - (maxWidth / 2);
								static float tickWidth = 0.f;
								static float barWidth = 0.f;
								tickWidth = (g_GlobalInfo.m_nShifted * Vars::Misc::CL_Move::DtbarOutlineWidth.m_Var);
								barWidth = g_Draw.EaseIn(barWidth, tickWidth, 0.9f);

								g_Draw.Rect(dtOffset - 1, (g_ScreenSize.h / 2) + 49, maxWidth + 2,
													Vars::Misc::CL_Move::DtbarOutlineHeight.m_Var + 2,
													{ 40, 40, 40, 180}); //yes the values will be hardcoded
								g_Draw.GradientRect(dtOffset, (g_ScreenSize.h / 2) + 50, dtOffset + barWidth,
													(g_ScreenSize.h / 2) + 50 + Vars::Misc::CL_Move::DtbarOutlineHeight.
													m_Var, color1, color2, true);
							}

							else if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 2) //nitro style
							{
								const float rratio = (static_cast<float>(g_GlobalInfo.m_nShifted) / static_cast<float>(
								Vars::Misc::CL_Move::DTTicks.m_Var));
								static float ratio = 0.f;
								ratio = g_Draw.EaseIn(ratio, rratio, 0.9f);

								if (ratio > 1.f) { ratio = 1.f; }
								else if (ratio < 0.f) { ratio = 0.f; }
								const int xoff = Vars::Misc::CL_Move::DTBarX.m_Var; // -20  width offset 
								const int yoff = Vars::Misc::CL_Move::DTBarY.m_Var; // 180  height offset
								const int yscale = Vars::Misc::CL_Move::DTBarScaleY.m_Var; //  12  height
								const int xscale = Vars::Misc::CL_Move::DTBarScaleX.m_Var; //  100
								static Color_t color5;
								{
									color5 = Colors::DTBarNitroIndicator;
								}


								g_Draw.Rect(g_ScreenSize.c - (80 / 2 + 2) + xoff, nY - (8 / 2 + 2) + (yoff + 20), 80 + 2,
											8 + 2, { 17, 24, 26, 255 });

								g_Draw.Rect(g_ScreenSize.c - (80 / 2) + xoff, nY - (8 / 2) + (yoff + 20), 80,
											8, { 17, 24, 26, 255 });
								

								//g_Draw.Rect(g_ScreenSize.c - (80 / 2) + xoff, nY - (8 / 2) + (yoff + 20),
								//					((g_ScreenSize.c - (80 / 2) + xoff) + (80 * ratio)),
								//					(nY - (80 / 2) + (yoff + 20) + 80), { color5 }, TRUE);

								if (g_GlobalInfo.m_nShifted == 0)
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 48) - 10 + yoff, {255, 255, 255, 255}, ALIGN_REVERSE,
												  L"Ticks 0/%i", Vars::Misc::CL_Move::DTTicks.m_Var, Vars::Misc::CL_Move::DTTicks.m_Var);
								}
								else if (g_GlobalInfo.m_bRecharging)
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale /2) + xoff + xscale),
												  nY - (yscale / 2 + 48) - 10 + yoff, {255, 255, 255, 255}, ALIGN_REVERSE,
												  L"Ticks %i/%i", g_GlobalInfo.m_nShifted, Vars::Misc::CL_Move::DTTicks.m_Var);
								}
								else if (g_GlobalInfo.m_nShifted > 0)
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 48) - 10 + yoff, {255, 255, 255, 255}, ALIGN_REVERSE,
												  L"Ticks %i/%i", g_GlobalInfo.m_nShifted, Vars::Misc::CL_Move::DTTicks.m_Var);
								}
							}
							/*
    							if (g_GlobalInfo.m_nShifted == 0) // no charge no money
    							{
    								g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
    								              nY - (yscale / 2 + 48) - 10 + yoff, {255, 55, 40, 255}, ALIGN_REVERSE,
    								              L"(0/%i) No Ticks!", Vars::Misc::CL_Move::DTTicks.m_Var, Vars::Misc::CL_Move::DTTicks.m_Var);
    							}
    							else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nWaitForShift || ratio < 1)) // charging 
    							{
    								g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
									              nY - (yscale / 2 + 48) - 10 + yoff, {255, 126, 0, 255}, ALIGN_REVERSE,
									              L"(%i/%i) Recharging!", Vars::Misc::CL_Move::DTTicks.m_Var, Vars::Misc::CL_Move::DTTicks.m_Var);
								}
    							else if (!g_GlobalInfo.m_nWaitForShift || ratio != 1) // activates when ready
    							{
    								g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
    								              nY - (yscale / 2 + 48) - 10 + yoff, {66, 255, 0, 255}, ALIGN_REVERSE,
    								              L"(%i/%i) Ready!", Vars::Misc::CL_Move::DTTicks.m_Var, Vars::Misc::CL_Move::DTTicks.m_Var);
    							}
    							else // activates when waiting blah blah blahg
								{
    								g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
    								              nY - (yscale / 2 + 48) - 10 + yoff, {255, 46, 46, 255}, ALIGN_REVERSE,
    								              L"(%i/%i) Waiting!", Vars::Misc::CL_Move::DTTicks.m_Var, Vars::Misc::CL_Move::DTTicks.m_Var);
    							}
							*/

							// Rijin DT Bar
							else if (Vars::Misc::CL_Move::DTBarStyle.m_Var == 3)
							{
								g_DTBar.Run();
								// put this here so we don't move menu if we r using something else, no biggie
								const float rratio = (static_cast<float>(g_GlobalInfo.m_nShifted) / static_cast<float>(
									Vars::Misc::CL_Move::DTTicks.m_Var));
								static float ratio = 0.f;
								ratio = g_Draw.EaseIn(ratio, rratio, 0.9f);

								if (ratio > 1.f) { ratio = 1.f; }
								else if (ratio < 0.f) { ratio = 0.f; }
								//if the user changes ticks after charging we don't want it to be like sliding out of bounds, this stops that.

								// these are all vars in dp but fedware doesnt have the vars and i am not adding them 
								//		so i added them
								const int xoff = Vars::Misc::CL_Move::DTBarX.m_Var;
								// width offset (is it called width offset who knows)
								const int yoff = Vars::Misc::CL_Move::DTBarY.m_Var; // height offset
								const int yscale = Vars::Misc::CL_Move::DTBarScaleY.m_Var; // height of bar
								const int xscale = Vars::Misc::CL_Move::DTBarScaleX.m_Var; // width of bar

								g_Draw.OutlinedRect(g_ScreenSize.c - (xscale / 2 + 1) + xoff,
													nY - (yscale / 2 + 1) + yoff, (xscale + 2), (yscale + 2),
													Colors::DtOutline);
								g_Draw.Rect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff, xscale,
											yscale, { 17, 24, 26, 255 });
								g_Draw.GradientRect(g_ScreenSize.c - (xscale / 2) + xoff, nY - (yscale / 2) + yoff,
													((g_ScreenSize.c - (xscale / 2) + xoff) + (xscale * ratio)),
													(nY - (yscale / 2) + yoff + yscale), { color1 }, { color2 }, TRUE);
								g_Draw.String(FONT_INDICATORS, g_ScreenSize.c - (xscale / 2 + 1) + xoff,
											  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 255, 255, 255 }, ALIGN_DEFAULT,
											  _(L"CHARGE"));
								if (g_GlobalInfo.m_nShifted == 0) // no charge no money
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 55, 40, 255 }, ALIGN_REVERSE,
												  _(L"NO CHARGE"));
								}
								else if (g_GlobalInfo.m_bRecharging && (g_GlobalInfo.m_nWaitForShift || ratio < 1)) // charging 
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 126, 0, 255 }, ALIGN_REVERSE,
												  _(L"CHARGING"));
								}
								else if (!g_GlobalInfo.m_nWaitForShift || ratio != 1) // activates when ready
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 66, 255, 0, 255 }, ALIGN_REVERSE,
												  _(L"READY"));
								}
								else // activates when waiting blah blah blahg
								{
									g_Draw.String(FONT_INDICATORS, (g_ScreenSize.c - (xscale / 2) + xoff + xscale),
												  nY - (yscale / 2 + 1) - 10 + yoff, { 255, 46, 46, 255 }, ALIGN_REVERSE,
												  _(L"DT IMPOSSIBLE"));
								}
							}
						}
					}
				}

				// Build Date
				if (g_Menu.IsOpen)
				{
					g_Draw.String(FONT_MENU, 5, g_ScreenSize.h - 5 - Vars::Fonts::FONT_MENU::nTall.m_Var, { 116, 255, 48, 255 }, ALIGN_DEFAULT, _(__DATE__));
				}

				// Debug info
				if (Vars::Debug::DebugInfo.m_Var)
				{
					int yoffset = 0, xoffset = 0;
					if (const int localDamage = g_PR->GetDamageByIndex(I::Engine->GetLocalPlayer()))
					{
						g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "localDamage = %d", localDamage);
					}

					if (const auto& pWeapon = g_EntityCache.m_pLocalWeapon)
					{
						if (const int weaponid = pWeapon->GetWeaponID())
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "weaponid = %i", weaponid);
						}
						if (const int weaponindex = pWeapon->GetItemDefIndex())
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "weaponindex = %i", weaponindex);
						}
						if (const int iviewmodel = pWeapon->GetiViewModelIndex())
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "iviewmodel = %i", iviewmodel);
						}
						if (const int nviewmodel = pWeapon->GetnViewModelIndex())
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "nviewmodel = %i", nviewmodel);
						}
					}

					if (const auto& pLocal = g_EntityCache.m_pLocal)
					{
						if (const int tickbase = pLocal->GetTickBase())
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "tickbase = %i", tickbase);
						}
						if (const int tickcount = I::GlobalVars->tickcount)
						{
							g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "tickcount = %i", tickcount);
							//float predictedsimtime = TICKS_TO_TIME(tickcount);
							//g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "predictedsimtime = %.1f", predictedsimtime);
							//yoffset += 20;
							//float simtime = pLocal->GetSimulationTime();
							//if (simtime) {
							//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "simtime = %.1f", simtime);
							//	yoffset += 20;
							//	float simtimedelta = predictedsimtime - simtime;
							//	{
							//		Color_t clr = { 255, 255, 255, 255 };
							//		if (simtimedelta > 0) { clr = { 255, 0, 246, 255 }; }
							//		else if (simtimedelta < 0) { clr = { 255, 139, 26, 255 }; }
							//		g_Draw.String(FONT_MENU, xoffset, yoffset, clr, ALIGN_DEFAULT, "simtimedelta = %.1f", simtimedelta);
							//		yoffset += 20;
							//	}
							//}
						}
						//int sequence = pLocal->m_nSequence();		// big numbah, maybe more useful when looking at individual anim layers?
						//if (sequence) {
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "sequence = %i", sequence);
						//	yoffset += 20;
						//}
						//int animtime = pLocal->m_flAnimTime();	// unused??? always returns the same value
						//if (animtime) {
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "animtime = %i", animtime);
						//	yoffset += 20;
						//}
						//float laggedmovement = pLocal->m_flLaggedMovementValue(); 
						//{
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "laggedmovement = %+.1f", laggedmovement);
						//	yoffset += 20;
						//}
						//float cycle = pLocal->m_flCycle();
						//{
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "cycle = %+.1f", cycle);
						//	yoffset += 20;
						//}
						//float playbackrate = pLocal->m_flPlaybackRate();
						//{
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "playbackrate = %+.1f", playbackrate);
						//	yoffset += 20;
						//}
						//bool clientanimations = pLocal->m_bClientSideAnimation();
						//{
						//	Color_t clr = clientanimations ? Color_t{ 108, 255, 0, 255 } : Color_t{ 255, 118, 36, 255 };
						//	g_Draw.String(FONT_MENU, xoffset, yoffset, clr, ALIGN_DEFAULT, "client animating");
						//	yoffset += 20;
						//}
						/*std::array poseparam = pLocal->GetPoseParam(); // 0 & 1, viewangles, 4 & 5, movement. and the other 20 entries do nothing?????? n1 valve
						int n = 0;
						for (; n < 24; n++) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "poseparam[%i] = %+.1f", n, poseparam[n]);
							yoffset += 20;
						}*/

						for (const auto& player : g_EntityCache.GetGroup(EGroupType::PLAYERS_ALL))
						{
							if (player == pLocal) { continue; }
							xoffset += 140;
							yoffset = 0;

							PlayerInfo_t pi{};
							if (I::Engine->GetPlayerInfo(player->GetIndex(), &pi))
							{
								if (!pi.fakeplayer)
								{
									const char* name = pi.name;
									if (name)
									{
										g_Draw.String(FONT_MENU, xoffset, yoffset, { 84, 0, 255, 255 }, ALIGN_DEFAULT, name);
									}
								}
								else
								{
									g_Draw.String(FONT_MENU, xoffset, yoffset, { 255, 0, 156, 255 }, ALIGN_DEFAULT, "server-bot");
								}

								if (!player->IsAlive())
								{
									// dead players should not show up here
									g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 80, 80, 80, 255 }, ALIGN_DEFAULT, "DEAD");
								}
							}

							if (const int tickcount = I::GlobalVars->tickcount)
							{
								g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "S : %i", tickcount);
								if (const int tickcountplayer = TIME_TO_TICKS(player->GetSimulationTime()))
								{
									g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "P : %i", tickcountplayer);
								}
							}

							//if (const auto rgflcoords = Player->GetRgflCoordinateFrame())
							//{
							//	for (int x = 1; x < 4; x++) {
							//		for (int y = 1; y < 5; y++) {
							//			if (rfglcoords[x][y]) {
							//				g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "[%i][%i] : %+.1f", x, y, rfglcoords[x][y]);
							//			}
							//		}
							//	}
							//}
							const auto& worldspace = player->GetAbsOrigin();
							if (!worldspace.IsZero())
							{
								g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "abs : [%+.1f] [%+.1f] [%+.1f]", worldspace.x, worldspace.y, worldspace.z);
							}
							const auto& mins = player->GetCollideableMins();
							const auto& maxs = player->GetCollideableMaxs();
							if (!mins.IsZero() && !maxs.IsZero())
							{
								g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "min : [%+.1f] [%+.1f] [%+.1f]", mins.x, mins.y, mins.z);
								g_Draw.String(FONT_MENU, xoffset, yoffset += 20, { 255, 255, 255, 255 }, ALIGN_DEFAULT, "max : [%+.1f] [%+.1f] [%+.1f]", maxs.x, maxs.y, maxs.z);
							}
						}
					}

					/*for (const auto& Projectile : g_EntityCache.GetGroup(EGroupType::WORLD_PROJECTILES))
					{
						Vec3 CollideableMins = Projectile->GetCollideableMins();
						Vec3 CollideableMaxs = Projectile->GetCollideableMaxs();
						if (!CollideableMins.IsZero()) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "mins = %+.2f %+.2f %+.2f", CollideableMins.x, CollideableMins.y, CollideableMins.z);
							yoffset += 20;
						}
						if (!CollideableMaxs.IsZero()) {
							g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "maxs = %+.2f %+.2f %+.2f", CollideableMaxs.x, CollideableMaxs.y, CollideableMaxs.z);
							yoffset += 20;
						}

						model_t* pModel = Projectile->GetModel();
						if (pModel) {
							studiohdr_t* pHDR = I::ModelInfo->GetStudioModel(pModel);
							if (pHDR) {
								g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "hullmin = %+.2f %+.2f %+.2f", pHDR->hull_min.x, pHDR->hull_min.y, pHDR->hull_min.z);
								yoffset += 20;
								g_Draw.String(FONT_MENU, xoffset, yoffset, { 255,255,255,255 }, ALIGN_DEFAULT, "hullmax = %+.2f %+.2f %+.2f", pHDR->hull_max.x, pHDR->hull_max.y, pHDR->hull_max.z);
								yoffset += 20;
							}
						}
					}*/
				}

				//Current Active Aimbot FOV
				if (Vars::Visuals::AimFOVAlpha.m_Var && g_GlobalInfo.m_flCurAimFOV)
				{
					if (const auto& pLocal = g_EntityCache.m_pLocal)
					{
						const float flFOV = static_cast<float>(Vars::Visuals::FieldOfView.m_Var);
						const float flR = tanf(DEG2RAD(g_GlobalInfo.m_flCurAimFOV) / 2.0f)
							/ tanf(
							DEG2RAD((pLocal->IsScoped() && !Vars::Visuals::RemoveZoom.m_Var) ? 30.0f : flFOV) /
							2.0f) * g_ScreenSize.w;
						const Color_t clr = Colors::FOVCircle;
						g_Draw.OutlinedCircle(g_ScreenSize.w / 2, g_ScreenSize.h / 2, flR, 68, clr);
					}
				}
			};
			OtherDraws();
			
			g_ESP.Run();
			g_Visuals.PickupTimers();
			g_SpyWarning.Run();
			g_PlayerArrows.Run();
			g_Followbot.Draw();
			g_SpectatorList.Run();
			g_CritHack.Draw();
			g_Radar.Run();

			if (Vars::AntiHack::AntiAim::Active.m_Var)
			{
				if (const auto& pLocal = g_EntityCache.m_pLocal)
				{
					static constexpr Color_t realColour = { 0, 255,0, 255 };
					static constexpr Color_t fakeColour = { 255, 0, 0, 255 };

					const auto& vOrigin = pLocal->GetAbsOrigin();

					Vec3 vScreen1, vScreen2;
					if (Utils::W2S(vOrigin, vScreen1))
					{
						constexpr auto distance = 50.f;
						if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, g_GlobalInfo.m_vRealViewAngles.y, distance), vScreen2))
							g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, realColour);

						if (Utils::W2S(Utils::GetRotatedPosition(vOrigin, g_GlobalInfo.m_vFakeViewAngles.y, distance), vScreen2))
							g_Draw.Line(vScreen1.x, vScreen1.y, vScreen2.x, vScreen2.y, fakeColour);
					}
				}
			}

			// you can use it for more, i'm sure. - myzarfin
			g_Notifications.Think();

			if (const auto& pLocal = g_EntityCache.m_pLocal)
			{
				if (pLocal->IsScoped() && Vars::Visuals::RemoveScope.m_Var && Vars::Visuals::ScopeLines.m_Var)
				{
					g_Visuals.ScopeLines();
				}
			}
		}

		static ConVar* localplayer_visionflags = I::CVars->FindVar("localplayer_visionflags");
		if (localplayer_visionflags)
		{
			switch (Vars::Visuals::Vision.m_Var)
			{
				case 1:
					localplayer_visionflags->SetValue(1);
					break;
				case 2:
					localplayer_visionflags->SetValue(2);
					break;
				case 3:
					localplayer_visionflags->SetValue(4);
					break;
				default:
					break;
			}
		}

		FinishDrawing(I::Surface);
	}
}