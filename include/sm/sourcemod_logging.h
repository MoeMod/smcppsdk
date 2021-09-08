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

		inline void LogMessage(LogLevel level, const char* message, ...)
		{
			const char* sLevel = LogLevelStr(level);
			char buffer[2048];
			va_list ap;
			va_start(ap, message);
			vsnprintf(buffer, sizeof(buffer), message, ap);
			va_end(ap);

			smutils->LogMessage(myself, "%s: %s", sLevel, buffer);
		}
	}
}