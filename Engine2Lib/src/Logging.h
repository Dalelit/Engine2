#pragma once

#define E2_LOG_INFO(msg) Logging::LogInfo(msg);
#define E2_LOG_WARNING(msg) Logging::LogWarning(msg);
#define E2_LOG_ERROR(msg) Logging::LogError(msg);

namespace Engine2
{
	class Logging
	{
	public:

		static void LogInfo(const char* msg);
		static inline void LogInfo(const std::string& msg) { LogInfo(msg.c_str()); }
		static inline void LogInfo(const std::stringstream& msg) { LogInfo(msg.str().c_str()); }

		static void LogWarning(const char* msg);
		static inline void LogWarning(const std::string& msg) { LogInfo(msg.c_str()); }
		static inline void LogWarning(const std::stringstream& msg) { LogInfo(msg.str().c_str()); }

		static void LogError(const char* msg);
		static inline void LogError(const std::string& msg) { LogInfo(msg.c_str()); }
		static inline void LogError(const std::stringstream& msg) { LogInfo(msg.str().c_str()); }

	protected:

	};
}