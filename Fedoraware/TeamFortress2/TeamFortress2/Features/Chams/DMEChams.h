#pragma once
#include "../../SDK/SDK.h"

class CDMEChams
{
public:
	IMaterial* m_pMatShaded;
	IMaterial* m_pMatShiny;
	IMaterial* m_pMatFlat;
	IMaterial* m_pMatFresnelHands;
	IMaterial* m_pMatFresnelPlayers;
	IMaterial* m_pMatFresnelWeap;
	IMaterial* m_pMatFresnel;
	IMaterial* m_pMatBrick;
	IMaterial* m_pMatScuffed;
	IMaterial* m_pMatWFShaded;
	IMaterial* m_pMatWFShiny;
	IMaterial* m_pMatWFFlat;
	IMaterial* m_pMatProxy;
private:
	bool ShouldRun();
	IMaterial* fetchMaterial(Chams_t chams);
	std::map<CBaseEntity*, bool> m_DrawnEntities;

public:
	bool HasDrawn(CBaseEntity* pEntity)
	{
		return m_DrawnEntities.find(pEntity) != m_DrawnEntities.end();
	}
	void Init();
	bool Render(const DrawModelState_t& pState, const ModelRenderInfo_t& pInfo, matrix3x4* pBoneToWorld);
};

inline CDMEChams g_DMEChams;
