#include "../Hooks.h"

#include "../../Features/Chams/Chams.h"
#include "../../Features/Chams/DMEChams.h"
#include "../../Features/Glow/Glow.h"
#include "../../Features/Backtrack/Backtrack.h"
#include "../../Features/Visuals/FakeAngleManager/FakeAng.h"

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
void DrawFakeAngles(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo);

MAKE_HOOK(ModelRender_DrawModelExecute, Utils::GetVFuncPtr(I::ModelRender, 19), void, __fastcall,
		  void* ecx, void* edx, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	CBaseEntity* pEntity = I::EntityList->GetClientEntity(pInfo.m_nEntIndex);

	DrawBT(ecx, edx, pEntity, pState, pInfo, pBoneToWorld);
	DrawFakeAngles(ecx, edx, pEntity, pState, pInfo);

	//if ((F::Chams.HasDrawn(pEntity) || F::Glow.HasDrawn(pEntity)) && !F::Glow.m_bDrawingGlow) { return; }
	//if (F::DMEChams.Render(pState, pInfo, pBoneToWorld)) { return; }

	if (!F::Glow.m_bRendering) {
		if (F::DMEChams.Render(pState, pInfo, pBoneToWorld)) { return; }
	}

	//if (F::Glow.HasDrawn(pEntity) && !F::Glow.m_bDrawingGlow) { return; }

	Hook.Original<FN>()(ecx, edx, pState, pInfo, pBoneToWorld);
}

void DrawBT(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo,
			matrix3x4* pBoneToWorld)
{
	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (Vars::Backtrack::Enabled.Value && Vars::Backtrack::BtChams::Enabled.Value && G::CurWeaponType != EWeaponType::PROJECTILE)
	{
		if (pEntity && pEntity->GetClassID() == ETFClassID::CTFPlayer)
		{
			if (!F::Glow.m_bRendering && !F::Chams.m_bRendering)
			{
				bool bMatWasForced = false;

				if (Vars::Backtrack::BtChams::EnemyOnly.Value && g_EntityCache.GetLocal() && pEntity->GetTeamNum() ==
					g_EntityCache.GetLocal()->GetTeamNum())
				{
					return;
				}

				I::ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
																 {
																	 switch (Vars::Backtrack::BtChams::Material.Value)
																	 {
																		 case 0:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatShaded;
																		 }
																		 case 1:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatShiny;
																		 }
																		 case 2:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatFlat;
																		 }
																		 case 3:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFShaded;
																		 }
																		 case 4:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFShiny;
																		 }
																		 case 5:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFFlat;
																		 }
																		 case 6:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatOverlay;
																		 }
																		 case 7:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatBrick;
																		 }
																		 default: return nullptr;
																	 }
																 }());
				if (bMatWasForced)
				{
					I::RenderView->SetColorModulation(
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.r),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.g),
						Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.b));
				}


				if (const auto& pRenderContext = I::MatSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.Value)
						pRenderContext->DepthRange(0.0f, 0.2f);
				}

				I::RenderView->SetBlend(Color::TOFLOAT(Vars::Backtrack::BtChams::BacktrackColor.a));



				if (Vars::Backtrack::BtChams::LastOnly.Value)
				{
					if (!F::Backtrack.Record[pEntity->GetIndex()].empty())
					{
						OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&F::Backtrack.Record[pEntity->GetIndex()].back().BoneMatrix));
					}
				}
				else
				{
					if (!F::Backtrack.Record[pEntity->GetIndex()].empty())
					{
						for (size_t t = 0; t < F::Backtrack.Record[pEntity->GetIndex()].size(); t++)
						{
							if (F::Backtrack.IsGoodTick(t)) { continue; }
							OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&F::Backtrack.Record[pEntity->GetIndex()].at(t).BoneMatrix));
						}
					}
				}


				bMatWasForced = true;

				if (bMatWasForced)
				{
					I::ModelRender->ForcedMaterialOverride(nullptr);
					I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				I::RenderView->SetBlend(1.0f);

				if (const auto& pRenderContext = I::MatSystem->GetRenderContext())
				{
					if (Vars::Backtrack::BtChams::IgnoreZ.Value)
						pRenderContext->DepthRange(0.0f, 1.0f);
				}
			}
		}
	}
}

void DrawFakeAngles(void* ecx, void* edx, CBaseEntity* pEntity, const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo)
{
	auto OriginalFn = Hooks::ModelRender_DrawModelExecute::Hook.Original<Hooks::ModelRender_DrawModelExecute::FN>();

	if (Vars::Misc::CL_Move::Fakelag.Value && Vars::Misc::CL_Move::FakelagIndicator.Value && F::FakeAng.DrawChams)
	{
		if (pEntity && pEntity == g_EntityCache.GetLocal())
		{
			if (!F::Glow.m_bRendering && !F::Chams.m_bRendering)
			{
				bool bMatWasForced = false;

				I::ModelRender->ForcedMaterialOverride([&]() -> IMaterial*
																 {
																	 switch (Vars::Misc::CL_Move::FLGChams::Material.Value)
																	 {
																		 case 0:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatShaded;
																		 }
																		 case 1:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatShiny;
																		 }
																		 case 2:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatFlat;
																		 }
																		 case 3:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFShaded;
																		 }
																		 case 4:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFShiny;
																		 }
																		 case 5:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatWFFlat;
																		 }
																		 case 6:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatOverlay;
																		 }
																		 case 7:
																		 {
																			 bMatWasForced = true;
																			 return F::DMEChams.m_pMatBrick;
																		 }
																		 default: return nullptr;
																	 }
																 }());
				if (bMatWasForced)
				{
					I::RenderView->SetColorModulation(
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.r),
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.g),
						Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.b));
				}

				I::RenderView->SetBlend(Color::TOFLOAT(Vars::Misc::CL_Move::FLGChams::FakelagColor.a)); // this is so much better than having a seperate alpha slider lmao
				OriginalFn(ecx, edx, pState, pInfo, reinterpret_cast<matrix3x4*>(&F::FakeAng.BoneMatrix));

				bMatWasForced = true;

				if (bMatWasForced)
				{
					I::ModelRender->ForcedMaterialOverride(nullptr);
					I::RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
				}

				I::RenderView->SetBlend(1.0f);
			}
		}
	}
}
