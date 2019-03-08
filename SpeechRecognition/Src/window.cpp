
#include"window.hpp"

#include<vector>

#include<Windows.h>

#include"Input.hpp"
#include"log.hpp"
#include"utils.hpp"

namespace Prizm
{
	namespace Window
	{
		HINSTANCE	instance;
		HWND		window_handle;
		int			screen_width, screen_height;
		bool		multi_touch_enable;
		bool        app_exit;

		void initRawInputDevices(void)
		{
			// register mouse for raw input
			// https://msdn.microsoft.com/en-us/library/windows/desktop/ms645565.aspx
			RAWINPUTDEVICE rid[1];
			rid[0].usUsagePage = (USHORT)0x01;	// HID_USAGE_PAGE_GENERIC;
			rid[0].usUsage = (USHORT)0x02;	// HID_USAGE_GENERIC_MOUSE;
			rid[0].dwFlags = 0;
			rid[0].hwndTarget = window_handle;

			// Cast between semantically different integer types : a Boolean type to HRESULT.
			if (FALSE == (RegisterRawInputDevices(rid, 1, sizeof(RAWINPUTDEVICE))))
				OutputDebugStringA("Failed to register raw input device!");

			// get devices and print info
			UINT num_devices = 0;
			GetRawInputDeviceList(
				NULL, &num_devices, sizeof(RAWINPUTDEVICELIST));
			if (num_devices == 0) return;

			std::vector<RAWINPUTDEVICELIST> device_list(num_devices);
			GetRawInputDeviceList(
				&device_list[0], &num_devices, sizeof(RAWINPUTDEVICELIST));

			std::vector<wchar_t> device_name_data;
			std::wstring device_name;
			for (UINT i = 0; i < num_devices; ++i)
			{
				const RAWINPUTDEVICELIST& device = device_list[i];

				if (device.dwType == RIM_TYPEMOUSE)
				{
					char info[1024];
					sprintf_s(info, "Mouse: Handle=0x%08p\n", device.hDevice);
					OutputDebugStringA(info);

					UINT data_size = 0;
					GetRawInputDeviceInfoA(
						device.hDevice, RIDI_DEVICENAME, nullptr, &data_size);

					if (data_size)
					{
						device_name_data.resize(data_size);
						UINT result = GetRawInputDeviceInfoA(
							device.hDevice, RIDI_DEVICENAME, &device_name_data[0], &data_size);

						if (result != UINT_MAX)
						{
							device_name.assign(device_name_data.begin(), device_name_data.end());

							char info[1024];
							std::string ndeviceName(device_name.begin(), device_name.end());
							sprintf_s(info, "  Name=%s\n", ndeviceName.c_str());
							OutputDebugStringA(info);
						}
					}

					RID_DEVICE_INFO device_info;
					device_info.cbSize = sizeof device_info;
					data_size = sizeof device_info;
					UINT result = GetRawInputDeviceInfoA(
						device.hDevice, RIDI_DEVICEINFO, &device_info, &data_size);

					if (result != UINT_MAX)
					{
						char info[1024];
						sprintf_s(info,
							"  Id=%u, Buttons=%u, SampleRate=%u, HorizontalWheel=%s\n",
							device_info.mouse.dwId,
							device_info.mouse.dwNumberOfButtons,
							device_info.mouse.dwSampleRate,
							device_info.mouse.fHasHorizontalWheel ? "1" : "0");
						OutputDebugStringA(info);
					}
				}
			}
		}
	};

	LRESULT __stdcall wndProc(HWND window_handle, UINT message, WPARAM w_param, LPARAM l_param)
	{
		switch (message)
		{
		case WM_CREATE:
			break;
		case WM_CLOSE:
			if (MessageBoxA(window_handle, "Quit ?", "User Notification", MB_YESNO | MB_DEFBUTTON2) == IDYES)
			{
				Log::info("[EXIT] BUTTON DOWN x");
				PostQuitMessage(0);
			}
			break;

			// keyboards
		case WM_KEYDOWN:
			Input::keyDown(static_cast<KeyCode>(w_param));
			if (Input::isKeyTriggered("F1") && !Input::isMouseCaptured()) Input::captureMouse(window_handle, true);
			break;

		case WM_KEYUP:
			Input::keyUp(static_cast<KeyCode>(w_param));
			break;

			// mouse buttons
		case WM_MBUTTONDOWN:
		case WM_LBUTTONDOWN:
		case WM_RBUTTONDOWN:
			if (Input::isMouseCaptured())	Input::keyDown(static_cast<KeyCode>(w_param));
			break;

		case WM_MBUTTONUP:
		case WM_RBUTTONUP:
		case WM_LBUTTONUP:
			if (Input::isMouseCaptured()) Input::keyUp(static_cast<KeyCode>(w_param));
			break;

		case WM_INPUT:
		{
			UINT raw_input_size = 48;
			BYTE* input_buffer[48];
			memset(input_buffer, 0, raw_input_size);

			GetRawInputData(
				(HRAWINPUT)l_param,
				RID_INPUT,
				input_buffer,
				&raw_input_size,
				sizeof(RAWINPUTHEADER));

			RAWINPUT* raw = (RAWINPUT*)input_buffer;

			if (raw->header.dwType == RIM_TYPEMOUSE && raw->data.mouse.usFlags == MOUSE_MOVE_RELATIVE)
			{
				if (Input::isMouseCaptured())
					Input::updateMousePos(raw->data.mouse.lLastX, raw->data.mouse.lLastY, raw->data.mouse.usButtonData);
			}
			break;
		}

		default:
			return DefWindowProcA(window_handle, message, w_param, l_param);
		}
		return 0;
	}

	bool Window::initialize(void)
	{
		app_exit = false;

		WNDCLASSEX wc;

		memset(&wc, 0, sizeof(wc));

		wc.cbSize = sizeof(wc);
		wc.style = CS_HREDRAW | CS_VREDRAW;
		wc.lpfnWndProc = wndProc;
		wc.hInstance = GetModuleHandleA(nullptr);
		wc.hIcon = LoadIconA(0, IDI_APPLICATION);
		wc.hCursor = LoadCursorA(0, IDC_ARROW);
		wc.hbrBackground = static_cast<HBRUSH>(GetStockObject(DKGRAY_BRUSH));
		wc.lpszClassName = window_caption;
		wc.hIconSm = LoadIconA(0, IDI_APPLICATION);

		RegisterClassExA(&wc);

		RECT window_rect = { 0, 0, window_width<long>, window_height<long> };
		AdjustWindowRectEx(&window_rect, WS_OVERLAPPEDWINDOW, true, 0);

		screen_width = window_rect.right - window_rect.left;
		screen_height = window_rect.bottom - window_rect.top;

		RECT desktop_rect;
		GetWindowRect(GetDesktopWindow(), &desktop_rect);

		int x = desktop_rect.right < window_rect.right ? 0 : (desktop_rect.right - screen_width) / 2;
		int y = desktop_rect.bottom < window_rect.bottom ? 0 : (desktop_rect.bottom - screen_height) / 2;

		window_handle = CreateWindowExA(0, window_caption, window_caption,
			WS_OVERLAPPEDWINDOW - (WS_MAXIMIZEBOX + WS_THICKFRAME),
			x, y, screen_width, screen_height, 0, 0, GetModuleHandleA(nullptr), 0);

		if (window_handle == nullptr)
		{
			Log::error("Can't create window. Window handle is null pointer.");
			PostQuitMessage(0);
			return false;
		}

		initRawInputDevices();

		ShowWindow(window_handle, SW_SHOW);

		Log::info("Window initialize succeeded.");

		return true;
	}

	void Window::finalize(void)
	{
		DestroyWindow(window_handle);
		window_handle = nullptr;

		UnregisterClassA(window_caption, instance);
		instance = nullptr;

		Log::info("Window finalize succeeded. Bye~~~");
	}

	HWND Window::getWindowHandle(void)
	{
		return window_handle;
	}
}