#include "general/pch.h"

#include "Log.h"

ae::WarningMessage::WarningMessage(const std::string& file, uint32_t line)
	: LogMessage(file, line)
{
	*this << "[WARNING] | File: " << GetFileName() << " | Line: " << m_Line << "\n'";
}

ae::WarningMessage::~WarningMessage()
{
}

void ae::WarningMessage::Log() const
{
	ae::Console::GetInstance().SetForegroundColor(ae::ConsoleForegroundColor::LightYellow);
	ae::Console::GetInstance().SetBackgroundColor(ae::ConsoleBackgroundColor::Black);

	std::cout << m_Message.str() << "'\n" << std::endl;
}
