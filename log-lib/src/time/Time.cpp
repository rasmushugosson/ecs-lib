#include "general/pch.h"

void ae::Time::Wait(double seconds)
{
#ifdef AE_WINDOWS
	Sleep(static_cast<DWORD>(seconds * 1000.0));
#else
	AE_THROW_RUNTIME_ERROR("Time::Wait is not implemented for this platform");
#endif // AE_WINDOWS
}
