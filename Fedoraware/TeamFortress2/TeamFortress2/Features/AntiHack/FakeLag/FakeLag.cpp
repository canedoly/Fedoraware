#include "FakeLag.h"
#include "../../Visuals/FakeAngleManager/FakeAng.h"

bool CFakeLag::IsAllowed(CBaseEntity* pLocal) {

	const int doubleTapAllowed = 22 - G::ShiftedTicks;
	const bool retainFakelagTest = Vars::Misc::CL_Move::RetainFakelag.Value ? G::ShiftedTicks != 1 : !G::ShiftedTicks;

	// Failsafe, in case we're trying to choke too many ticks
	if (ChokeCounter > 22) {
		return false;
	}

	if (ChokeCounter >= ChosenAmount) {
		return false;
	}

	// Are we attacking? TODO: Add more logic here
	if (G::IsAttacking) {
		return false;
	}

	// Do we have enough velocity for velocity mode?
	if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Velocity && pLocal->GetVecVelocity().Length2D() < 20.f)
	{
		return false;
	}

	// Are we recharging or shifting ticks?
	if (ChokeCounter >= doubleTapAllowed || G::Recharging || G::RechargeQueued || G::ShouldShift || !retainFakelagTest) {
		return false;
	}

	return true;
}

void CFakeLag::OnTick(CUserCmd* pCmd, bool* pSendPacket) {
	G::IsChoking = false;	//	do this first
	if (!Vars::Misc::CL_Move::Fakelag.Value) { return; }

	// Set the selected choke amount (if not random)
	if (Vars::Misc::CL_Move::FakelagMode.Value != FL_Random) {
		ChosenAmount = Vars::Misc::CL_Move::FakelagValue.Value;
	}

	const auto& pLocal = g_EntityCache.GetLocal();
	if (!pLocal || !pLocal->IsAlive())
	{

		*pSendPacket = true;
		ChokeCounter = 0;

		return;
	}

	// Are we even allowed to choke?
	if (!IsAllowed(pLocal)) {
		*pSendPacket = true;
		// Set a new random amount (if desired)
		if (Vars::Misc::CL_Move::FakelagMode.Value == FL_Random) { ChosenAmount = Utils::RandIntSimple(Vars::Misc::CL_Move::FakelagMin.Value, Vars::Misc::CL_Move::FakelagMax.Value); }
		ChokeCounter = 0;
		return;
	}

	G::IsChoking = true;
	*pSendPacket = false;
	ChokeCounter++;
}