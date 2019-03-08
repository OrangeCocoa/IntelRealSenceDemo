
# This project dependent by IntelRealSenceSDK.
I gave it a try IntelRealSenceSDK.

## SDK download
https://software.intel.com/en-us/realsense-sdk-windows-eol<br>
2016 R2 FullSDK

## Project setting
You must edit solution property.<br>
"VC++ directory - include directory" column write your install "Intel\RSSDK\include" and "Intel\RSSDK\sample\common\include" directory path.<br>
And "VC++ directory - library directory" column write "Intel\RSSDK\lib\Win32" and "Intel\RSSDK\sample\common\lib\Win32\\$(PlatformToolset)" directory path.

"linker - input - addition dependency file" column write "libpxc_d.lib" and "libpxcmd_d.lib" when debug mode, "libpxc.lib" and "libpxcmd.lib" when release mode.