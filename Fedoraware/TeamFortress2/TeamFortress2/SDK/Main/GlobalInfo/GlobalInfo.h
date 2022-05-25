#pragma once

#include "../BaseEntity/BaseEntity.h"

#define DT_WAIT_CALLS 0;

struct VelFixRecord {
	Vec3 m_vecOrigin;
	int m_nFlags;
	float m_flSimulationTime;
};

struct DormantData {
	Vec3 Location;
	float LastUpdate = 0.f;
};

struct Priority {
	int Mode = 2;
};

struct GlobalInfo_t
{
	int m_nCurrentTargetIdx				= 0;
	int m_nCurItemDefIndex              = 0;
	int m_nWaitForShift                 = Vars::Misc::CL_Move::DTWaitCalls;
	int m_nShifted                      = 0;
	int gNotifCounter = 0;
	int vEyeAngDelay = 25	;
	bool m_bWeaponCanHeadShot			= false;
	bool m_bWeaponCanAttack				= false;
	bool m_bWeaponCanSecondaryAttack	= false;
	bool m_bAAActive					= false;
	bool m_bFakeShotPitch				= false;
	bool m_bHitscanSilentActive			= false;
	bool m_bAvoidingBackstab			= false;
	bool m_bProjectileSilentActive		= false; //flamethrower
	bool m_bAutoBackstabRunning			= false;
	bool m_bHitscanRunning				= false;
	bool m_bSilentTime					= false;
	bool m_bLocalSpectated				= false;
	bool m_bRollExploiting			    = false;
	bool m_bAttacking			 	    = false; // this is only used by aimbot, and is also set to false at the start of a lot of functions, this is not reliable
	bool m_bModulateWorld				= true;
	bool m_bShouldShift                 = false;
	bool m_bRecharging                  = false;
	bool m_bRechargeQueued				= false;
	bool m_bForceSendPacket				= false; // might not actually be useful 
	bool m_bForceChokePacket			= false; // might not actually be useful 
	bool m_bChoking						= false; // might not actually be useful 
	bool unloadWndProcHook = false;
	float m_flCurAimFOV					= 0.0f;
	Vec3 m_vPredictedPos				= {};
	Vec3 m_vAimPos						= {};
	VMatrix m_WorldToProjection			= {};
	Vec3 m_vViewAngles					= {};
	Vec3 m_vRealViewAngles				= {};
	Vec3 m_vFakeViewAngles				= {};
	Vec3 m_vPunchAngles					= {};
	Vec3 linearPredLine = {}; //clubpenguin > tf2
	EWeaponType m_WeaponType			= {};
	CUserCmd* currentUserCmd{ nullptr };
	CUserCmd* lateUserCmd{ nullptr };
	std::map < CBaseEntity*, VelFixRecord> velFixRecord;
	std::vector<Vec3> predBeforeLines;
	std::vector<Vec3> predFutureLines;
	bool m_bFreecamActive				= false;
	Vec3 m_vFreecamPos					= {};
	std::map<int, DormantData> partyPlayerESP;		// < Player-Index, DormantData >
	std::map<int, int> chokeMap;
	int tickShiftQueue = 0; // Ticks that shouls be shifted
	bool m_bDrawingStaticProps = false;
	std::map<uint32_t, Priority> PlayerPriority;

	bool IsIgnored(uint32_t friendsID)
	{
		return PlayerPriority[friendsID].Mode < 2;
	}
};

inline GlobalInfo_t g_GlobalInfo;
