#include "../Hooks.h"

#include "../../Features/Killstreak/Killstreak.h"
#include "../../Features/Fedworking/Fedworking.h"
#include "../../Features/CritHack/CritHack.h"

MAKE_HOOK(GameEventManager_FireEventClientSide, Utils::GetVFuncPtr(I::GameEventManager, 8), bool, __fastcall,
		  void* ecx, void* edx, CGameEvent* pEvent)
{
	const FNV1A_t uNameHash = FNV1A::Hash(pEvent->GetName());
	g_CritHack.FireEvent(pEvent, uNameHash);
	F::Killstreaker.FireEvents(pEvent, uNameHash);

	if (uNameHash == FNV1A::HashConst("party_chat"))
	{
		const auto msg = pEvent->GetString("text");

		// Handle networking
		if (Utils::StartsWith(msg, "FED@"))
		{
			if (Vars::Misc::PartyNetworking.Value)
			{
				F::Fedworking.HandleMessage(msg);
			}
			return false;
		}
	}

	return Hook.Original<FN>()(ecx, edx, pEvent);
}