#include "DMEChams.h"

#include "../Vars.h"
#include "../../Hooks/ModelRenderHook/ModelRenderHook.h"
#include "Chams.h"
#include "../Glow/Glow.h"
#include "../Backtrack/Backtrack.h"





// I can't believe i'm doing this

// TODO: add an option to import custom ones from a file path

namespace ProxySkins
{
	static IMaterial* spectrumSplattered = nullptr;
	static IMaterial* electroSkullsBlue = nullptr;
	static IMaterial* frozenAurora = nullptr;
	static IMaterial* jazzy = nullptr;
	static IMaterial* hana = nullptr;
	static IMaterial* wtf = nullptr;
	static IMaterial* ghost = nullptr;
	static IMaterial* flames = nullptr;
	static IMaterial* spookwood = nullptr;
	static IMaterial* edgy = nullptr;
	static IMaterial* serenity = nullptr;
	static IMaterial* fade = nullptr;

	void Init()
	{
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2020/2203845847/2203845847_spectrum");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			spectrumSplattered = g_Interfaces.MatSystem->Create("spectrumskinrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2018/1326382486/1326382486_electro_skulls_blu");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			electroSkullsBlue = g_Interfaces.MatSystem->Create("elecrtoskullsrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/smissmas_2017/1193300219/1193300219_cow");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			frozenAurora = g_Interfaces.MatSystem->Create("frozenaurorarargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/smissmas_2020/1558054217/1558054217_a");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			jazzy = g_Interfaces.MatSystem->Create("jazzyrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/smissmas_2017/1183962036/1183962036_aes_blue");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			hana = g_Interfaces.MatSystem->Create("hanarargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2021/2596228713/2596228713_main_blu");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			wtf = g_Interfaces.MatSystem->Create("wtfrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2021/2594850983/2594850983_pattern_red");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			ghost = g_Interfaces.MatSystem->Create("ghostrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2020/2223065529/2223065529_helldriver_flames");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			flames = g_Interfaces.MatSystem->Create("flamesrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/screamfortress_2018/1180839618/1180839618_spooky_wood");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			spookwood = g_Interfaces.MatSystem->Create("spookwoodrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/500642896/500642896");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			edgy = g_Interfaces.MatSystem->Create("edgyrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/smissmas_2020/2265040710/2265040710_starlight2");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			serenity = g_Interfaces.MatSystem->Create("serenityrargh", kv);
		}
		{
			auto kv = new KeyValues("VertexLitGeneric");
			kv->SetString("$basetexture", "patterns/workshop/smissmas_2020/2265040710/2265040710_serenity");
			kv->SetString("$additive", "1");
			kv->SetString("$scrollanglevar", "0");
			KeyValues* Proxies = new KeyValues("Proxies");
			{
				g_KeyValUtils.Initialize(Proxies, (char*)"proxies");
				{
					KeyValues* Sine = new KeyValues("Sine");
					g_KeyValUtils.Initialize(Sine, (char*)"sine");
					Sine->SetString("resultvar", "$scrollanglevar");
					Sine->SetString("sineperiod", "50000");
					Sine->SetString("sinemin", "0");
					Sine->SetString("sinemax", "360");
					Proxies->AddSubkey(Sine);
				}
				{
					KeyValues* TextureScroll = new KeyValues("TextureScroll");
					g_KeyValUtils.Initialize(TextureScroll, (char*)"texturescroll");
					TextureScroll->SetString("texturescrollvar", "$BasetextureTransform");
					TextureScroll->SetString("texturescrollrate", "0.1");
					TextureScroll->SetString("texturescrollangle", "$scrollanglevar");
					Proxies->AddSubkey(TextureScroll);
				}
				kv->AddSubkey(Proxies);

			}
			fade = g_Interfaces.MatSystem->Create("faderargh", kv);
		}
	}
	IMaterial* getProxy(int var);
}

bool CDMEChams::ShouldRun()
{
	if (!Vars::Chams::DME::Active.m_Var || g_Interfaces.EngineVGui->IsGameUIVisible() || !Vars::Chams::Main::Active.
		m_Var)
		return false;

	return true;
}

void CDMEChams::Init()
{
	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShaded = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatShiny = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatFlat = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatFlat", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.05 0.1]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[0.5 0.5 0]");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$envmaptint", "[1 1 1]");
		m_pMatFresnelHands = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatFresnelHands", kv);
		m_pMatFresnelPlayers = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatFresnelPlayers", kv);
		m_pMatFresnelWeap = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatFresnelWeapons", kv);
		m_pMatFresnel = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatFresnel", kv);
		// having seperate materials for all of these seems to fix an issue with them breaking each other, mayb there is another fix?
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "brick/brickwall031b");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$color2", "[10 10 10]");
		kv->SetString("$additive", "1");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[4 4 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "10");
		m_pMatBrick = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatBrick", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$basetexture", "models/player/shared/ice_player");
		kv->SetString("$bumpmap", "models/player/shared/shared_normal");
		kv->SetString("$translucent", "1");
		kv->SetString("$additive", "1");
		kv->SetString("$phong", "1");
		kv->SetString("$phongfresnelranges", "[0 0.5 10]");
		kv->SetString("$phongtint", "[0 0 0]");
		kv->SetString("$envmap", "skybox/sky_dustbowl_01");
		kv->SetString("$envmapfresnel", "1");
		kv->SetString("$envmaptint", "[0 0 0]");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumtint", "[0 0 0]");
		kv->SetString("$rimlight", "1");
		kv->SetString("$rimlightboost", "10");
		m_pMatScuffed = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatScuffed", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShaded = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatWFShaded", kv);
	}

	{
		auto kv = new KeyValues("VertexLitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		kv->SetString("$bumpmap", "vgui/white_additive");
		kv->SetString("$envmap", "cubemaps/cubemap_sheen001");
		kv->SetString("$selfillum", "1");
		kv->SetString("$selfillumfresnel", "1");
		kv->SetString("$selfillumfresnelminmaxexp", "[-0.25 1 1]");
		m_pMatWFShiny = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatWFShiny", kv);
	}

	{
		auto kv = new KeyValues("UnlitGeneric");
		kv->SetString("$wireframe", "1");
		kv->SetString("$basetexture", "vgui/white_additive");
		m_pMatWFFlat = g_Interfaces.MatSystem->Create("DME_MAT_m_pMatWFFlat", kv);
	}

	ProxySkins::Init();
}

Chams_t fetchChams(CBaseEntity* pEntity) {
	PlayerInfo_t info{}; g_Interfaces.Engine->GetPlayerInfo(pEntity->GetIndex(), &info);

	if (pEntity->GetIndex() == g_GlobalInfo.m_nCurrentTargetIdx)
		return Vars::Chams::Players::Target;
	if (pEntity == g_EntityCache.m_pLocal)
		return Vars::Chams::Players::Local;
	if (g_EntityCache.Friends[pEntity->GetIndex()] || pEntity == g_EntityCache.m_pLocal)
		return Vars::Chams::Players::Friend;
	if (pEntity->GetTeamNum() != g_EntityCache.m_pLocal->GetTeamNum())
		return Vars::Chams::Players::Enemy;
	if (pEntity->GetTeamNum() == g_EntityCache.m_pLocal->GetTeamNum())
		return Vars::Chams::Players::Team;
}

IMaterial* CDMEChams::fetchMaterial(Chams_t chams)
{
	switch (chams.drawMaterial)
	{
	case 1:
		return m_pMatShaded;
	case 2:
		return m_pMatShiny;
	case 3:
		return m_pMatFlat;
	case 4:
		return m_pMatWFShaded;
	case 5:
		return m_pMatWFShiny;
	case 6:
		return m_pMatWFFlat;
	case 7:
		return m_pMatFresnel;
	case 8:
		return m_pMatBrick;
	default:
		return nullptr;
	}
}

IMaterial* ProxySkins::getProxy(int var) {
	switch (var) {
	case 1:
		return spectrumSplattered;
	case 2:
		return electroSkullsBlue;
	case 3:
		return frozenAurora;
	case 4:
		return jazzy;
	case 5:
		return hana;
	case 6:
		return wtf;
	case 7:
		return ghost;
	case 8:
		return flames;
	case 9:
		return spookwood;
	case 10:
		return edgy;
	case 11:
		return serenity;
	case 12:
		return fade;
	default: 
		return nullptr;
	}
}

bool CDMEChams::Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld)
{
	m_bRendering = false;
	bool found1 = true;
	bool found2 = true;
	bool found3 = true;
	bool found4 = true;
	auto pLocal = g_EntityCache.m_pLocal;
	if (pLocal) {
		if (IMatRenderContext* pRenderContext = g_Interfaces.MatSystem->GetRenderContext()) {

			static IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar(_("$selfillumtint"), &found1);
			static IMaterialVar* envmap = m_pMatFresnel->FindVar(_("$envmaptint"), &found2);
			static IMaterialVar* pVar = m_pMatScuffed->FindVar(_("$phongtint"), &found3);
			static IMaterialVar* pVar2 = m_pMatScuffed->FindVar(_("$envmaptint"), &found4);

			ShaderStencilState_t StencilState = {};
			StencilState.m_bEnable = true;
			StencilState.m_nReferenceValue = 1;
			StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
			StencilState.m_PassOp = STENCILOPERATION_REPLACE;
			StencilState.m_FailOp = STENCILOPERATION_KEEP;
			StencilState.m_ZFailOp = STENCILOPERATION_REPLACE;
			StencilState.SetStencilState(pRenderContext);

			if (ShouldRun())
			{
				m_bRendering = true;
				CBaseEntity* pEntity = g_Interfaces.EntityList->GetClientEntity(pInfo.m_nEntIndex);

				if (pEntity && pEntity->GetClassID() == ETFClassID::CTFViewModel)
				{
					bool bMatWasForced = false;
					auto chams = Vars::Chams::Players::Arms;
					auto cMat = fetchMaterial(chams);

					g_Interfaces.ModelRender->ForcedMaterialOverride(cMat);
					if (cMat != nullptr)
						bMatWasForced = true;

					if (chams.drawMaterial != 7)
					{
						if (Vars::Chams::DME::HandsRainbow.m_Var) // this looks like trash bro
						{
							g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Utils::Rainbow().r),
								Color::TOFLOAT(Utils::Rainbow().g),
								Color::TOFLOAT(Utils::Rainbow().b));
						}
						else
						{
							g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Hands.r),
								Color::TOFLOAT(Colors::Hands.g),
								Color::TOFLOAT(Colors::Hands.b));
						}
					}
					else if (chams.drawMaterial == 7)
					{
						if (found1)
						{
							fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(chams.fresnelBaseColor.r) * 4,
								Color::TOFLOAT(chams.fresnelBaseColor.g) * 4,
								Color::TOFLOAT(chams.fresnelBaseColor.b) * 4);
						}
						if (found2)
						{
							envmap->SetVecValue(Color::TOFLOAT(Colors::Hands.r) * 4, Color::TOFLOAT(Colors::Hands.g) * 4,
								Color::TOFLOAT(Colors::Hands.b) * 4);
						}
					}

					g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(Colors::Hands.a));



					ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
						ModelRenderHook::DrawModelExecute::index)
						(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
					bMatWasForced = true;


					if (Vars::Chams::DME::HandsProxySkin.m_Var && bMatWasForced)
					{

						IMaterial* pMaterial = nullptr;
						{
							pMaterial = ProxySkins::getProxy(Vars::Chams::DME::HandsProxySkin.m_Var);
						}
						pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Vars::Chams::DME::HandsProxyWF.m_Var);
						g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
						g_Interfaces.ModelRender->ForcedMaterialOverride(pMaterial);

						ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
							ModelRenderHook::DrawModelExecute::index)
							(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
					}

					if (chams.overlayType && bMatWasForced)
					{
						// Overlay
						if (found3)
						{
							if (Vars::Chams::DME::HandsOverlayRainbow.m_Var)
							{
								pVar->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
									Color::TOFLOAT(Utils::Rainbow().g) * 4,
									Color::TOFLOAT(Utils::Rainbow().b) * 4);
							}
							else
							{
								pVar->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
									Color::TOFLOAT(chams.overlayColor.g),
									Color::TOFLOAT(chams.overlayColor.b));
							}
						}
						if (found4)
						{
							if (Vars::Chams::DME::HandsOverlayRainbow.m_Var)
							{
								pVar2->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
									Color::TOFLOAT(Utils::Rainbow().g) * 4,
									Color::TOFLOAT(Utils::Rainbow().b) * 4);
							}
							else
							{
								pVar2->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
									Color::TOFLOAT(chams.overlayColor.g),
									Color::TOFLOAT(chams.overlayColor.b));
							}
						}
						m_pMatScuffed->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chams.overlayType == 2);

						g_Interfaces.ModelRender->ForcedMaterialOverride(m_pMatScuffed);

						ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
							ModelRenderHook::DrawModelExecute::index)
							(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
					}


					if (bMatWasForced)
					{
						g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
						g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
					}

					g_Interfaces.RenderView->SetBlend(1.0f);

					return true;
				}
				if (pEntity && !pEntity->GetDormant() && pEntity->IsPlayer() && Vars::Chams::DME::Active.m_Var)
				{
					bool bMatWasForced = false;
					bool bIsLocal = pEntity->GetIndex() == g_Interfaces.Engine->GetLocalPlayer();
					static auto pLocal = g_EntityCache.m_pLocal;
					auto chams = fetchChams(pEntity);
					Color_t DrawColor = Utils::GetEntityDrawColor(pEntity, Vars::ESP::Main::EnableTeamEnemyColors.m_Var);

					if (fetchMaterial(chams) == nullptr)
						bMatWasForced = false;

					if (chams.drawMaterial) // handles chams
					{
						g_Interfaces.ModelRender->ForcedMaterialOverride(fetchMaterial(chams));

						if (chams.drawMaterial != 7)
						{
							g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(DrawColor.r),
								Color::TOFLOAT(DrawColor.g),
								Color::TOFLOAT(DrawColor.b));
						}
						else
						{
							if (found1)
							{
								fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(chams.fresnelBaseColor.r),
									Color::TOFLOAT(chams.fresnelBaseColor.g),
									Color::TOFLOAT(chams.fresnelBaseColor.b));
							}
							if (found2)
							{
								envmap->SetVecValue(Color::TOFLOAT(DrawColor.r), Color::TOFLOAT(DrawColor.g),
									Color::TOFLOAT(DrawColor.b));
							}
						}
					}

					if (chams.showObstructed)
						pRenderContext->DepthRange(0.0f, 0.2f);

					g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(DrawColor.a));
					ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
						ModelRenderHook::DrawModelExecute::index)
						(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
					bMatWasForced = true;


					if (chams.overlayType && bMatWasForced) // handles glow overlay
					{
						if (found3)
						{
							pVar->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
								Color::TOFLOAT(chams.overlayColor.g),
								Color::TOFLOAT(chams.overlayColor.b));
						}
						if (found4)
						{

							pVar2->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
								Color::TOFLOAT(chams.overlayColor.g),
								Color::TOFLOAT(chams.overlayColor.b));
						}
						m_pMatScuffed->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chams.overlayType == 2);
						g_Interfaces.ModelRender->ForcedMaterialOverride(m_pMatScuffed);

						ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
							ModelRenderHook::DrawModelExecute::index)
							(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
					}

					g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
					g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);

					g_Interfaces.RenderView->SetBlend(1.0f);

					if (chams.showObstructed)
						pRenderContext->DepthRange(0.0f, 1.0f);

					m_DrawnEntities[pEntity] = true;
					return true;
				}
				if (!pEntity && pInfo.m_pModel)
				{
					std::string_view szModelName(g_Interfaces.ModelInfo->GetModelName(pInfo.m_pModel));

					if (szModelName.find(_("weapon")) != std::string_view::npos
						&& szModelName.find(_("arrow")) == std::string_view::npos
						&& szModelName.find(_("w_syringe")) == std::string_view::npos
						&& szModelName.find(_("nail")) == std::string_view::npos
						&& szModelName.find(_("shell")) == std::string_view::npos
						&& szModelName.find(_("parachute")) == std::string_view::npos
						&& szModelName.find(_("buffbanner")) == std::string_view::npos
						&& szModelName.find(_("shogun_warbanner")) == std::string_view::npos
						&& szModelName.find(_("targe")) == std::string_view::npos //same as world model, can't filter
						&& szModelName.find(_("shield")) == std::string_view::npos //same as world model, can't filter
						&& szModelName.find(_("repair_claw")) == std::string_view::npos)
					{
						//g_Interfaces.DebugOverlay->AddTextOverlay(pInfo.m_vOrigin, 0.003f, "%hs", szModelName);

						bool bMatWasForced = false;
						auto chams = Vars::Chams::Players::Weapon;
						auto cMat = fetchMaterial(chams);

						if (!chams.chamsActive)
							return false;


						int _case = 0;

						if (Vars::Chams::DME::Weapon.m_Var)
						{
							g_Interfaces.ModelRender->ForcedMaterialOverride(cMat);
						}

						if (bMatWasForced && (chams.drawMaterial != 7))
						{
							if (Vars::Chams::DME::WeaponRainbow.m_Var)
							{
								g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Utils::Rainbow().r),
									Color::TOFLOAT(Utils::Rainbow().g),
									Color::TOFLOAT(Utils::Rainbow().b));
							}
							else
							{
								g_Interfaces.RenderView->SetColorModulation(Color::TOFLOAT(Colors::Weapon.r),
									Color::TOFLOAT(Colors::Weapon.g),
									Color::TOFLOAT(Colors::Weapon.b));
							}
						}

						g_Interfaces.RenderView->SetBlend(Color::TOFLOAT(Colors::Weapon.a));

						ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
							ModelRenderHook::DrawModelExecute::index) // base
							(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
						bMatWasForced = true;
						if (chams.drawMaterial == 7)
						{
							if (found1)
							{
								fresnelSelfillumtint->SetVecValue(Color::TOFLOAT(Colors::FresnelBaseWeps.r) * 4,
									Color::TOFLOAT(Colors::FresnelBaseWeps.g) * 4,
									Color::TOFLOAT(Colors::FresnelBaseWeps.b) * 4);
							}
							if (found2)
							{
								envmap->SetVecValue(Color::TOFLOAT(Colors::Weapon.r) * 4, Color::TOFLOAT(Colors::Weapon.g) * 4,
									Color::TOFLOAT(Colors::Weapon.b) * 4);
							}
						}

						if (Vars::Chams::DME::WeaponsProxySkin.m_Var && bMatWasForced)
						{

							IMaterial* pMaterial = nullptr;
							{
								pMaterial = ProxySkins::getProxy(Vars::Chams::DME::WeaponsProxySkin.m_Var);
							}
							pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, Vars::Chams::DME::HandsProxyWF.m_Var);
							g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
							g_Interfaces.ModelRender->ForcedMaterialOverride(pMaterial);

							ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
								ModelRenderHook::DrawModelExecute::index)
								(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
						}

						if (chams.overlayType && bMatWasForced)
						{
							IMaterial* pMaterial = m_pMatScuffed;
							bool found = false;
							bool found2 = false;
							bool found3 = false;
							IMaterialVar* pVar = pMaterial->FindVar(_("$phongtint"), &found);
							if (found)
							{
								if (Vars::Chams::DME::WeaponOverlayRainbow.m_Var)
								{
									pVar->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
										Color::TOFLOAT(Utils::Rainbow().g) * 4,
										Color::TOFLOAT(Utils::Rainbow().b) * 4);
								}
								else
								{
									pVar->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
										Color::TOFLOAT(chams.overlayColor.g),
										Color::TOFLOAT(chams.overlayColor.b));
								}
							}
							IMaterialVar* pVar2 = pMaterial->FindVar(_("$envmaptint"), &found2);
							if (found2)
							{
								if (Vars::Chams::DME::WeaponOverlayRainbow.m_Var)
								{
									pVar2->SetVecValue(Color::TOFLOAT(Utils::Rainbow().r) * 4,
										Color::TOFLOAT(Utils::Rainbow().g) * 4,
										Color::TOFLOAT(Utils::Rainbow().b) * 4);
								}
								else
								{
									pVar2->SetVecValue(Color::TOFLOAT(chams.overlayColor.r),
										Color::TOFLOAT(chams.overlayColor.g),
										Color::TOFLOAT(chams.overlayColor.b));
								}
							}
							pMaterial->SetMaterialVarFlag(MATERIAL_VAR_WIREFRAME, chams.overlayType == 2);

							g_Interfaces.ModelRender->ForcedMaterialOverride(pMaterial);

							ModelRenderHook::Table.Original<ModelRenderHook::DrawModelExecute::fn>(
								ModelRenderHook::DrawModelExecute::index) //overlay
								(g_Interfaces.ModelRender, pState, pInfo, pBoneToWorld);
						}

						if (bMatWasForced)
						{
							g_Interfaces.ModelRender->ForcedMaterialOverride(nullptr);
							g_Interfaces.RenderView->SetColorModulation(1.0f, 1.0f, 1.0f);
						}

						g_Interfaces.RenderView->SetBlend(1.0f);

						return true;
					}
				}

				m_bRendering = false;
			}
		}
	}
	return false;
}
