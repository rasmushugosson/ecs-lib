#include "general/pch.h"

#include "Log.h"

ae::InfoMessage::InfoMessage(const std::string& file, uint32_t line)
	: LogMessage(file, line)
{
	*this << "[INFO] | File: " << GetFileName() << " | Line: " << m_Line << "\n'";
}

ae::InfoMessage::~InfoMessage()
{
}

void ae::InfoMessage::Log() const
{
	ae::Console::GetInstance().SetForegroundColor(ae::ConsoleForegroundColor::Green);
	ae::Console::GetInstance().SetBackgroundColor(ae::ConsoleBackgroundColor::Black);

	std::cout << m_Message.str() << "'\n" << std::endl;
}
