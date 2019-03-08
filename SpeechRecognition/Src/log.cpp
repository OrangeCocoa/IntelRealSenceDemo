
#include"log.hpp"

#include<fstream>
#include<iostream>

#include<Windows.h>
#include<io.h>
#include<fcntl.h>

#include"utils.hpp"

namespace Prizm
{
	namespace Log
	{
		std::ofstream out_file;
		LogMode current_mode;
	}

	void Log::initialize(LogMode mode, std::string& current_dir)
	{
		switch (mode)
		{
		case LogMode::NONE: 
			return;

		case LogMode::CONSOLE:
			initializeConsole();
			break;

		case LogMode::FILE:
			initializeFile(current_dir);
			break;

		case LogMode::CONSOLE_AND_FILE:
			initializeConsole();
			initializeFile(current_dir);
			break;

		default:
			break;
		}

		current_mode = mode;
	}

	void Log::finalize(void)
	{
		std::string msg = StrUtils::getCurrentTimeAsStringWithBrackets() + "[Log] finalize()";
		if (out_file.is_open())
		{
			out_file << msg;
			out_file.close();
		}
		std::cout << msg;
		OutputDebugStringA(msg.c_str());

		if (current_mode == CONSOLE || current_mode == CONSOLE_AND_FILE)
		{
			FreeConsole();
		}
	}

	void Log::error(const std::string& s)
	{
		std::string err = StrUtils::getCurrentTimeAsStringWithBrackets() + "[ERROR]: " + s + "\n";

		OutputDebugStringA(err.c_str());	// vs
		
		if (out_file.is_open())
			out_file << err;				// file
		
		std::cout << err;					// console
	}

	void Log::warning(const std::string& s)
	{
		std::string warn = StrUtils::getCurrentTimeAsStringWithBrackets() + "[WARNING]: " + s + "\n";

		OutputDebugStringA(warn.c_str());	// vs

		if (out_file.is_open())
			out_file << warn;				// file

		std::cout << warn;					// console
	}

	void Log::info(const std::string& s)
	{
		std::string info = StrUtils::getCurrentTimeAsStringWithBrackets() + "[INFO]: " + s + "\n";

		OutputDebugStringA(info.c_str());	// vs

		if (out_file.is_open())
			out_file << info;				// file

		std::cout << info;					// console
	}

	void Log::initializeConsole(void)
	{
		// src: https://stackoverflow.com/a/46050762/2034041

		AllocConsole();

		// Get STDOUT handle
		HANDLE console_output = GetStdHandle(STD_OUTPUT_HANDLE);
		int system_output = _open_osfhandle(intptr_t(console_output), _O_TEXT);
		std::FILE *output_handle = _fdopen(system_output, "w");

		// Get STDERR handle
		HANDLE console_error = GetStdHandle(STD_ERROR_HANDLE);
		int system_error = _open_osfhandle(intptr_t(console_error), _O_TEXT);
		std::FILE *error_handle = _fdopen(system_error, "w");

		// Get STDIN handle
		HANDLE console_input = GetStdHandle(STD_INPUT_HANDLE);
		int system_input = _open_osfhandle(intptr_t(console_input), _O_TEXT);
		std::FILE *input_handle = _fdopen(system_input, "r");

		//make cout, wcout, cin, wcin, wcerr, cerr, wclog and clog point to console as well
		std::ios::sync_with_stdio(true);

		// Redirect the CRT standard input, output, and error handles to the console
		freopen_s(&input_handle, "CONIN$", "r", stdin);
		freopen_s(&output_handle, "CONOUT$", "w", stdout);
		freopen_s(&error_handle, "CONOUT$", "w", stderr);

		std::wcout.clear();
		std::cout.clear();
		std::wcerr.clear();
		std::cerr.clear();
		std::wcin.clear();
		std::cin.clear();
	}

	void Log::initializeFile(std::string& current_dir)
	{
		const std::string log_directory = current_dir + "\\Logs";

		std::string err_msg = "";

		if (CreateDirectoryA(current_dir.c_str(), nullptr))
		{
			if (CreateDirectoryA(log_directory.c_str(), nullptr))
			{
				std::string file_name = StrUtils::getCurrentTimeAsString() + "_PrizmEngine_Log.txt";

				out_file.open(log_directory + "//" + file_name);

				if (out_file)
				{
					std::string msg = StrUtils::getCurrentTimeAsStringWithBrackets() + "[Log] " + "Log initialize Done.";
					out_file << msg;
					std::cout << msg << std::endl;
				}
				else
				{
					err_msg = "Cannot open log file " + file_name;
				}
			}
			else
			{
				err_msg = "Failed to create directory " + log_directory;
			}
		}
		else
		{
			err_msg = "Failed to create directory " + current_dir;
		}

		if (!err_msg.empty())
		{
			MessageBoxA(NULL, err_msg.c_str(), "PrizmEngine: Error Initializing Logging", MB_OK);
		}
	}
}