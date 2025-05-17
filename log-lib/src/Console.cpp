#include "general/pch.h"

#include "Log.h"

ae::Console ae::Console::m_Instance;

ae::Console::Console()
	: m_ForegroundColor(ConsoleForegroundColor::White)
	, m_BackgroundColor(ConsoleBackgroundColor::Black)
{}

ae::Console::~Console()
{
#ifdef AE_WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(ConsoleForegroundColor::White) | static_cast<WORD>(ConsoleBackgroundColor::Black));
#endif // AE_WINDOWS
}

void ae::Console::SetForegroundColor(ConsoleForegroundColor textColor)
{
	m_ForegroundColor = textColor;
	Update();
}

void ae::Console::SetBackgroundColor(ConsoleBackgroundColor backgroundColor)
{
	m_BackgroundColor = backgroundColor;
	Update();
}

void ae::Console::Update() const
{
#ifdef AE_WINDOWS
	SetConsoleTextAttribute(GetStdHandle(STD_OUTPUT_HANDLE), static_cast<WORD>(m_ForegroundColor) | static_cast<WORD>(m_BackgroundColor));
#endif // AE_WINDOWS
}
