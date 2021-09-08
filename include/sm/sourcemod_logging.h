#pragma once

#include "extension.h"

namespace sm
{
	inline namespace logging
	{
		enum LogLevel
		{
			LEVEL_OFF,
			LEVEL_FATAL,
			LEVEL_ERROR,
			LEVEL_WARN,
			LEVEL_INFO,
			LEVEL_DEBUG,
			LEVEL_TRACE,
			LEVEL_ALL,

		};

		inline const char* LogLevelStr(LogLevel level)
		{
			switch (level)
			{
			case LEVEL_FATAL: return "FATAL"; break;
			case LEVEL_ERROR: return "ERROR"; break;
			case LEVEL_WARN: return "WARN"; break;
			case LEVEL_INFO: return "INFO"; break;
			case LEVEL_DEBUG: return "DEBUG"; break;
			case LEVEL_TRACE: return "TRACE"; break;
			default: return "UNKNOWN"; break;
			}
		}

		inline void LogEvent(LogLevel level, const char* format, ...)
		{
			char buffer[2048];
			va_list ap;
			va_start(ap, message);
			vsnprintf(buffer, sizeof(buffer), message, ap);
			va_end(ap);

			switch (level)
			{
			case LEVEL_FATAL: fatal(buffer); break;
			case LEVEL_ERROR: error(buffer); break;
			case LEVEL_WARN: warn(buffer); break;
			case LEVEL_INFO: info(buffer); break;
			case LEVEL_DEBUG: debug(buffer); break;
			case LEVEL_TRACE: trace(buffer); break;
			case LEVEL_OFF: break; // Not logging here..
			case LEVEL_ALL: smutils->LogMessage(myself, "There are %d log levels here.", LEVEL_ALL); break;
			default: smutils->LogError(myself, "Unknown log level found: %d", level); break;
			}
		}

		inline void trace(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_TRACE);
			smutils->LogMessage(myself, "%s: %s", level, message);
		}

		inline void debug(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_DEBUG);
			smutils->LogMessage(myself, "%s: %s", level, message);
		}

		inline void info(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_INFO);
			smutils->LogMessage(myself, "%s: %s", level, message);
		}

		inline void warn(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_WARN);
			smutils->LogMessage(myself, "%s: %s", level, message);
		}

		inline void error(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_ERROR);
			smutils->LogError(myself, "%s: %s", level, message);
		}

		inline void fatal(const char* message)
		{
			const char* level = LogLevelStr(LEVEL_FATAL);
			smutils->LogError(myself, "%s: %s", level, message);
			abort();
		}
	}
}