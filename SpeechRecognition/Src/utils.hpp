#pragma once

#include<string>
#include<random>
#include<filesystem>

namespace Prizm
{
	constexpr bool succeeded(long hr) { return static_cast<int>(hr) >= 0; }
	constexpr bool failed(long hr) { return static_cast<int>(hr) < 0; }

	// true, false, TRUE, FALSE
	/*inline constexpr bool succeeded(long hr) { return static_cast<int>(hr) > 0; }
	inline constexpr bool failed(long hr) { return static_cast<int>(hr) <= 0; }*/

	namespace StrUtils
	{
		std::string unicodeToAscii(wchar_t* pwstr);
		std::wstring asciiToUnicode(const char* pstr);

		template<typename _StringType>
		std::string toString(_StringType str)
		{
			return std::filesystem::path(str).string();
		}

		std::string getCurrentTimeAsString(void);
		std::string getCurrentTimeAsStringWithBrackets(void);
	}

	namespace DirectoryUtils
	{
		enum FolderType
		{
			PROGRAM_FILE,	//	%ProgramFiles% (%SystemDrive%\Program Files)
			APPDATA,		//	%APPDATA% (%USERPROFILE%\AppData\Roaming)
			LOCAL_APPDATA,	//	%LOCALAPPDATA% (%USERPROFILE%\AppData\Local)
			USER_PROFILE,	//	%USERPROFILE% (%SystemDrive%\Users\%USERNAME%) 
			DOCUMENTS,		//	%USERPROFILE%\Documents
		};

		std::string getSpecialFolderPath(FolderType);
	}

	namespace Utils
	{
		float randF(float low, float high);
		int	randI(int low, int high);
		size_t randU(size_t low, size_t high);

		template<class T>
		T randomRange(T low, T high)
		{
			if (low > high)
			{// swap params
				T tmp = low;
				low = high;
				high = tmp;
			}

			thread_local std::mt19937_64 engine(std::random_device{}());

			std::uniform_real_distribution<T> distribution(low, high);

			return distribution(engine);
		}
	}
}
