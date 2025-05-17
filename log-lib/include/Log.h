#pragma once

#include <stdint.h>

#include <iostream>
#include <string>
#include <sstream>
#include <vector>

#include <exception>
#include <stdexcept>

#ifdef AE_WINDOWS
#include <Windows.h>
#endif

namespace ae
{
	class LogMessage
	{
	public:
		LogMessage(const std::string& file, uint32_t line);
		~LogMessage();

		template <typename T>
		LogMessage& operator<<(const T& value)
		{
			m_Message << value;
			return *this;
		}

		virtual void Log() const = 0;
	protected:
		std::string GetFileName() const;
	protected:
		std::string m_File;
		uint32_t m_Line;
		std::ostringstream m_Message;
	};

    class TraceMessage : public LogMessage
    {
    public:
        TraceMessage(const std::string& file, uint32_t line);
        ~TraceMessage();

        void Log() const override;
    };

    class InfoMessage : public LogMessage
    {
    public:
        InfoMessage(const std::string& file, uint32_t line);
        ~InfoMessage();

        void Log() const override;
    };

    class WarningMessage : public LogMessage
    {
    public:
        WarningMessage(const std::string& file, uint32_t line);
        ~WarningMessage();

        void Log() const override;
    };

    class ErrorMessage : public LogMessage
    {
    public:
        ErrorMessage(const std::string& file, uint32_t line);
        ~ErrorMessage();

        void Log() const override;
    };

    class FatalMessage : public LogMessage
    {
    public:
        FatalMessage(const std::string& file, uint32_t line);
        ~FatalMessage();

        void Log() const override;
    };

#ifdef AE_WINDOWS
    enum class ConsoleForegroundColor
	{
		Black = 0,
		Red = FOREGROUND_RED,
		Green = FOREGROUND_GREEN,
		Blue = FOREGROUND_BLUE,
		Yellow = FOREGROUND_RED | FOREGROUND_GREEN,
		Magenta = FOREGROUND_RED | FOREGROUND_BLUE,
		Cyan = FOREGROUND_GREEN | FOREGROUND_BLUE,
		LightGray = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE,
		Gray = FOREGROUND_INTENSITY,
		LightRed = FOREGROUND_RED | FOREGROUND_INTENSITY,
		LightGreen = FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		LightBlue = FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		LightYellow = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_INTENSITY,
		LightMagenta = FOREGROUND_RED | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		LightCyan = FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY,
		White = FOREGROUND_RED | FOREGROUND_GREEN | FOREGROUND_BLUE | FOREGROUND_INTENSITY
	};

	enum class ConsoleBackgroundColor
	{
		Black = 0,
		Red = BACKGROUND_RED,
		Green = BACKGROUND_GREEN,
		Blue = BACKGROUND_BLUE,
		Yellow = BACKGROUND_RED | BACKGROUND_GREEN,
		Magenta = BACKGROUND_RED | BACKGROUND_BLUE,
		Cyan = BACKGROUND_GREEN | BACKGROUND_BLUE,
		LightGray = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE,
		Gray = BACKGROUND_INTENSITY,
		LightRed = BACKGROUND_RED | BACKGROUND_INTENSITY,
		LightGreen = BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		LightBlue = BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		LightYellow = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_INTENSITY,
		LightMagenta = BACKGROUND_RED | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		LightCyan = BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY,
		White = BACKGROUND_RED | BACKGROUND_GREEN | BACKGROUND_BLUE | BACKGROUND_INTENSITY
	};
#else
    enum class ConsoleForegroundColor
	{
		Black = 0,
		Red,
		Green,
		Blue,
		Yellow,
		Magenta,
		Cyan,
		LightGray,
		Gray,
		LightRed,
		LightGreen,
		LightBlue,
		LightYellow,
		LightMagent,
		LightCyan,
		White
	};

	enum class ConsoleBackgroundColor
	{
		Black = 0,
		Red,
		Green,
		Blue,
		Yellow,
		Magenta,
		Cyan,
		LightGray,
		Gray,
		LightRed,
		LightGreen,
		LightBlue,
		LightYellow,
		LightMagent,
		LightCyan,
		White
	};
#endif // AE_WINDOWS

	class Console
	{
	private:
		Console();
	public:
		Console(const Console&) = delete;
		Console& operator=(const Console&) = delete;
		Console(Console&&) = delete;
		Console& operator=(Console&&) = delete;
		~Console();

		inline static Console& GetInstance()
		{
			return m_Instance;
		}

		void SetForegroundColor(ConsoleForegroundColor foregroundColor);
		void SetBackgroundColor(ConsoleBackgroundColor backgroundColor);
	private:
		void Update() const;
	private:
		ConsoleForegroundColor m_ForegroundColor;
		ConsoleBackgroundColor m_BackgroundColor;
	private:
		static Console m_Instance;
	};

#define AE_TRACE ae::TraceMessage(__FILE__, __LINE__)
#define AE_INFO ae::InfoMessage(__FILE__, __LINE__)
#define AE_WARNING ae::WarningMessage(__FILE__, __LINE__)
#define AE_ERROR ae::ErrorMessage(__FILE__, __LINE__)
#define AE_FATAL ae::FatalMessage(__FILE__, __LINE__)

#ifdef AE_DEBUG

#define AE_LOG_CONSOLE(l, m) (l << m).Log()
#define AE_LOG_CONSOLE_RELEASE(l, m)

#define AE_LOG_CONSOLE_NEWLINE() std::cout << std::endl
#define AE_LOG_CONSOLE_NEWLINE_RELEASE()

#elif AE_RELEASE // AE_DEBUG

#define AE_LOG_CONSOLE(l, m)
#define AE_LOG_CONSOLE_RELEASE(l, m) (l << m).Log()

#define AE_LOG_CONSOLE_NEWLINE() 
#define AE_LOG_CONSOLE_NEWLINE_RELEASE() std::cout << std::endl

#else // AE_DEBUG

#define AE_LOG_CONSOLE(l, m)
#define AE_LOG_CONSOLE_RELEASE(l, m)

#define AE_LOG_CONSOLE_NEWLINE()
#define AE_LOG_CONSOLE_NEWLINE_RELEASE()

#endif // AE_DEBUG

#define AE_LOG_CONSOLE_DEBUG AE_LOG_CONSOLE
#define AE_LOG_CONSOLE_NEWLINE_DEBUG AE_LOG_CONSOLE_NEWLINE

#ifdef AE_DEBUG
	
#define AE_LOG_CONSOLE_ALL(l, m) AE_LOG_CONSOLE_DEBUG(l, m)
#define AE_LOG_CONSOLE_NEWLINE_ALL AE_LOG_CONSOLE_NEWLINE_DEBUG

#elif AE_RELEASE // AE_DEBUG

#define AE_LOG_CONSOLE_ALL(l, m) AE_LOG_CONSOLE_RELEASE(l, m)
#define AE_LOG_CONSOLE_NEWLINE_ALL AE_LOG_CONSOLE_NEWLINE_RELEASE

#else // AE_DEBUG

#define AE_LOG_CONSOLE_ALL(l, m)
#define AE_LOG_CONSOLE_NEWLINE_ALL()

#endif // AE_DEBUG

	// Exceptions ---------------------------------------------------------------------------------------------------------------------------------------

	std::string FormatError(const std::string& type, const std::string& file, uint32_t line, const std::ostringstream& message);

	class LogicError : public std::logic_error
	{
	public:
		explicit LogicError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::logic_error(FormatError("Logic error", file, line, message))
		{
		}
	};

#define AE_THROW_LOGIC_ERROR(m) throw ae::LogicError(__FILE__, __LINE__, std::ostringstream() << m)	

	class InvalidArgument : public std::invalid_argument
	{
	public:
		explicit InvalidArgument(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::invalid_argument(FormatError("Invalid argument", file, line, message))
		{
		}
	};

#define AE_THROW_INVALID_ARGUMENT(m) throw ae::InvalidArgument(__FILE__, __LINE__, std::ostringstream() << m)

	class MathError : public std::domain_error
	{
	public:
		explicit MathError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::domain_error(FormatError("Math error", file, line, message))
		{
		}
	};

#define AE_THROW_MATH_ERROR(m) throw ae::MathError(__FILE__, __LINE__, std::ostringstream() << m)

	class LengthError : public std::length_error
	{
	public:
		explicit LengthError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::length_error(FormatError("Length error", file, line, message))
		{
		}
	};

#define AE_THROW_LENGTH_ERROR(m) throw ae::LengthError(__FILE__, __LINE__, std::ostringstream() << m)

	class OutOfRangeError : public std::out_of_range
	{
	public:
		explicit OutOfRangeError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::out_of_range(FormatError("Out of range error", file, line, message))
		{
		}
	};

#define AE_THROW_OUT_OF_RANGE_ERROR(m) throw ae::OutOfRangeError(__FILE__, __LINE__, std::ostringstream() << m)

	class RuntimeError : public std::runtime_error
	{
	public:
		explicit RuntimeError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Runtime error", file, line, message))
		{
		}
	};

#define AE_THROW_RUNTIME_ERROR(m) throw ae::RuntimeError(__FILE__, __LINE__, std::ostringstream() << m)

	class RangeError : public std::range_error
	{
	public:
		explicit RangeError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::range_error(FormatError("Range error", file, line, message))
		{
		}
	};

#define AE_THROW_RANGE_ERROR(m) throw ae::RangeError(__FILE__, __LINE__, std::ostringstream() << m)

	class OverflowError : public std::overflow_error
	{
	public:
		explicit OverflowError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::overflow_error(FormatError("Overflow error", file, line, message))
		{
		}
	};

#define AE_THROW_OVERFLOW_ERROR(m) throw ae::OverflowError(__FILE__, __LINE__, std::ostringstream() << m)

	class UnderflowError : public std::underflow_error
	{
	public:
		explicit UnderflowError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::underflow_error(FormatError("Underflow error", file, line, message))
		{
		}
	};

#define AE_THROW_UNDERFLOW_ERROR(m) throw ae::UnderflowError(__FILE__, __LINE__, std::ostringstream() << m)

	class FileNotFoundError : public std::runtime_error
	{
	public:
		explicit FileNotFoundError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("File not found error", file, line, message))
		{
		}
	};

#define AE_THROW_FILE_NOT_FOUND_ERROR(m) throw ae::FileNotFoundError(__FILE__, __LINE__, std::ostringstream() << m)

	class ResourceLoadError : public std::runtime_error
	{
	public:
		explicit ResourceLoadError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Resource load error", file, line, message))
		{
		}
	};

#define AE_THROW_RESOURCE_LOAD_ERROR(m) throw ae::ResourceLoadError(__FILE__, __LINE__, std::ostringstream() << m)

	class ResourceUnloadError : public std::runtime_error
	{
	public:
		explicit ResourceUnloadError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Resource unload error", file, line, message))
		{
		}
	};

#define AE_THROW_RESOURCE_UNLOAD_ERROR(m) throw ae::ResourceUnloadError(__FILE__, __LINE__, std::ostringstream() << m)

	class ResourceFormatError : public std::runtime_error
	{
	public:
		explicit ResourceFormatError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Resource format error", file, line, message))
		{
		}
	};

#define AE_THROW_RESOURCE_FORMAT_ERROR(m) throw ae::ResourceFormatError(__FILE__, __LINE__, std::ostringstream() << m)

	class ResourceNotFoundError : public std::runtime_error
	{
	public:
		explicit ResourceNotFoundError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Resource not found error", file, line, message))
		{
		}
	};

#define AE_THROW_RESOURCE_NOT_FOUND_ERROR(m) throw ae::ResourceNotFoundError(__FILE__, __LINE__, std::ostringstream() << m)

	class InvalidStateError : public std::runtime_error
	{
	public:
		explicit InvalidStateError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Invalid state error", file, line, message))
		{
		}
	};

#define AE_THROW_INVALID_STATE_ERROR(m) throw ae::InvalidStateError(__FILE__, __LINE__, std::ostringstream() << m)

	class InvalidOperationError : public std::runtime_error
	{
	public:
		explicit InvalidOperationError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Invalid operation error", file, line, message))
		{
		}
	};

#define AE_THROW_GRAPHICS_ERROR(m) throw ae::InvalidOperationError(__FILE__, __LINE__, std::ostringstream() << m)

	class GraphicsError : public std::runtime_error
	{
	public:
		explicit GraphicsError(const std::string& file, uint32_t line, const std::ostringstream& message)
			: std::runtime_error(FormatError("Graphics error", file, line, message))
		{
		}
	};

	// Time ---------------------------------------------------------------------------------------------------------------------------------------------

	class Time
	{
	public:
		static void Wait(double seconds);
	};

	class Timer
	{
	public:
		Timer();
		~Timer();

		void Start();
		void Stop();
		void Reset();

		double GetElapsedTime() const;
	private:
#ifdef AE_WINDOWS
		LARGE_INTEGER m_Frequency;
		LARGE_INTEGER m_StartTime;
		LARGE_INTEGER m_StopTime;
#endif // AE_WINDOWS
		double m_ElapsedTime;
		bool m_IsRunning;
	};
}
