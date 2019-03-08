#pragma once

#include<string>
#include<memory>
#include<windows.h>

namespace Prizm
{
	constexpr const char* window_caption = "study";

	template<typename T>
	constexpr T window_width = 600;

	template<typename T>
	constexpr T window_height = 480;

	namespace Window
	{
		bool initialize(void);
		void finalize(void);

		HWND getWindowHandle(void);
	};
}