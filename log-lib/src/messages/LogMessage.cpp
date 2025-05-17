#include "general/pch.h"

#include "Log.h"

ae::LogMessage::LogMessage(const std::string& file, uint32_t line)
	: m_File(file), m_Line(line), m_Message()
{
}

ae::LogMessage::~LogMessage()
{
}

std::string ae::LogMessage::GetFileName() const
{
	std::string fileName = m_File;
	size_t lastSlash = fileName.find_last_of("/\\");

	if (lastSlash != std::string::npos)
	{
		fileName = fileName.substr(lastSlash + 1);
	}

	return fileName;
}
