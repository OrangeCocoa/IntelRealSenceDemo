#pragma once

#include<string>
#include<Windows.h>

/*
Mouse and touch input has raw input data.
These need initialized in WinAPI's event handler.
*/

namespace Prizm
{
	using KeyCode = unsigned int;
	
	namespace Input
	{
		// multi touch num
		constexpr int max_touchcount = 2;

		void initialize(void);

		// mouse capture
		void captureMouse(HWND, bool do_capture);
		bool isMouseCaptured(void);
		POINT mouseCapturePosition(void);

		// update
		void keyDown(KeyCode);
		void keyUp(KeyCode);

		void buttonDown(KeyCode);
		void buttonUp(KeyCode);
		void updateMousePos(long, long, short);

		void updateTouchPos(long, long, int, DWORD);

		// key state
		bool isKeyPress(const char*);
		
		bool isKeyReleased(const char*);
		
		bool isKeyTriggered(const char*);

		// mouse state
		bool isMousePress(const char*);
		
		bool isScrollUp(void);
		bool isScrollDown(void);

		int mouseDeltaX(void);
		int mouseDeltaY(void);

		// touch state
		bool isTouchPress(int);
		bool isTouchMove(int);
		bool isTouchReleased(int);
		bool isTouchTriggered(int);

		int touchDeltaX(int);
		int touchDeltaY(int);

		const long* getMouseDelta(void);
		const long* getTouchDelta(int);

		// update end of frame
		void postStateUpdate(void);
	};
}