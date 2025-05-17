#include "general/pch.h"

ae::Timer::Timer()
	: m_Frequency(), m_StartTime(), m_StopTime(), m_ElapsedTime(0.0), m_IsRunning(false)
{
#ifndef AE_WINDOWS
	AE_THROW_RUNTIME_ERROR("Timer is not implemented for this platform");
#endif // !AE_WINDOWS

#ifdef AE_WINDOWS
	QueryPerformanceFrequency(&m_Frequency);
#endif // AE_WINDOWS
}

ae::Timer::~Timer()
{
}

void ae::Timer::Start()
{
#ifndef AE_WINDOWS
	AE_THROW_RUNTIME_ERROR("Timer::Start is not implemented for this platform");
#endif // !AE_WINDOWS

#ifdef AE_WINDOWS
	QueryPerformanceCounter(&m_StartTime);
#endif // AE_WINDOWS

	m_IsRunning = true;
}

void ae::Timer::Stop()
{
#ifndef AE_WINDOWS
	AE_THROW_RUNTIME_ERROR("Timer::Stop is not implemented for this platform");
#endif // !AE_WINDOWS

#ifdef AE_DEBUG
	if (!m_IsRunning)
	{
		AE_LOG_CONSOLE(AE_WARNING, "Tried to stop Timer but it is not running");
		return;
	}
#endif // AE_DEBUG

#ifdef AE_WINDOWS
	QueryPerformanceCounter(&m_StopTime);
	m_ElapsedTime = static_cast<float>(m_StopTime.QuadPart - m_StartTime.QuadPart) / static_cast<double>(m_Frequency.QuadPart);
#endif // AE_WINDOWS

	m_IsRunning = false;
}

void ae::Timer::Reset()
{
	m_ElapsedTime = 0.0;
	m_IsRunning = false;
}

double ae::Timer::GetElapsedTime() const
{
#ifndef AE_WINDOWS
	AE_THROW_RUNTIME_ERROR("Timer::GetElapsedTime is not implemented for this platform");
#endif // !AE_WINDOWS

	if (m_IsRunning)
	{
#ifdef AE_WINDOWS
		LARGE_INTEGER currentTime;
		QueryPerformanceCounter(&currentTime);
		return static_cast<double>(currentTime.QuadPart - m_StartTime.QuadPart) / static_cast<double>(m_Frequency.QuadPart);
#endif // AE_WINDOWS
	}

	return m_ElapsedTime;
}
