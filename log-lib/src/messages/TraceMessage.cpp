#include "general/pch.h"

#include "Log.h"

ae::TraceMessage::TraceMessage(const std::string& file, uint32_t line)
	: LogMessage(file, line)
{
	*this << "[TRACE] | File: " << GetFileName() << " | Line: " << m_Line << "\n'";
}

ae::TraceMessage::~TraceMessage()
{
}

void ae::TraceMessage::Log() const
{
	ae::Console::GetInstance().SetForegroundColor(ae::ConsoleForegroundColor::Gray);
	ae::Console::GetInstance().SetBackgroundColor(ae::ConsoleBackgroundColor::Black);

	std::cout << m_Message.str() << "'" << std::endl;
}
