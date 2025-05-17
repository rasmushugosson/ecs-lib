#include "general/pch.h"

std::string ae::FormatError(const std::string& type, const std::string& file, uint32_t line, const std::ostringstream& message)
{
	std::string fileName = "Unknown file";

	size_t lastSlash = file.find_last_of("/\\");
	if (lastSlash != std::string::npos)
	{
		fileName = file.substr(lastSlash + 1);
	}

	std::ostringstream oss;
	oss << type << " at ( File: " << fileName << " | Line: " << line << " ):\n - " << message.str();
	return oss.str();
}
