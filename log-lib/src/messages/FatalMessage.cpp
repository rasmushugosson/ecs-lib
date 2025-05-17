#include "general/pch.h"

#include "Log.h"

ae::FatalMessage::FatalMessage(const std::string& file, uint32_t line)
	: LogMessage(file, line)
{
	*this << "\n[FATAL ERROR] | File: " << GetFileName() << " | Line: " << m_Line << "\n'";
}

ae::FatalMessage::~FatalMessage()
{
}

void ae::FatalMessage::Log() const
{
	ae::Console::GetInstance().SetForegroundColor(ae::ConsoleForegroundColor::Red);
	ae::Console::GetInstance().SetBackgroundColor(ae::ConsoleBackgroundColor::Black);

	std::cerr << m_Message.str() << "'\n" << std::endl;
}
