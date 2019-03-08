#pragma once

#include<string>

namespace Prizm
{
	namespace Log
	{
		enum LogMode
		{
			NONE = 0,
			CONSOLE,
			FILE,
			CONSOLE_AND_FILE,
		};

		void initialize(LogMode, std::string&);

		void finalize(void);

		void error(const std::string&);

		template<class... Args>
		void error(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format.c_str(), args...);
			error(std::string(msg));
		}

		void warning(const std::string&);

		template<class... Args>
		void warning(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format.c_str(), args...);
			warning(std::string(msg));
		}

		void info(const std::string&);

		template<class... Args>
		void info(const std::string& format, Args&&... args)
		{
			char msg[128];
			sprintf_s(msg, format.c_str(), args...);
			info(std::string(msg));
		}

		void initializeConsole(void);

		void initializeFile(std::string&);
	};
}