rem the paths are relative to AmisCore's folder

echo Copying Ambulant DLL and Lib
IF EXIST ..\..\..\..\..\..\ambulant\bin\win32\libambulant_shwin32_D.dll copy ..\..\..\..\..\..\ambulant\bin\win32\libambulant_shwin32_D.dll ..\..\bin\libambulant_shwin32_D.dll /y
IF EXIST ..\..\..\..\..\..\ambulant\bin\win32\libambulant_shwin32.dll copy ..\..\..\..\..\..\ambulant\bin\win32\libambulant_shwin32.dll ..\..\bin\libambulant_shwin32.dll /y
IF EXIST ..\..\..\..\..\..\ambulant\lib\win32\libambulant_shwin32_D.lib copy ..\..\..\..\..\..\ambulant\lib\win32\libambulant_shwin32_D.lib ..\..\lib\libambulant_shwin32_D.lib /y
IF EXIST ..\..\..\..\..\..\ambulant\lib\win32\libambulant_shwin32.lib copy ..\..\..\..\..\..\ambulant\lib\win32\libambulant_shwin32.lib ..\..\lib\libambulant_shwin32.lib /y

echo Copying FFMpeg plugin
IF EXIST ..\..\..\..\..\..\ambulant\bin\win32\libampluginD_ffmpeg.dll copy ..\..\..\..\..\..\ambulant\bin\win32\libampluginD_ffmpeg.dll ..\..\bin\libampluginD_ffmpeg.dll /y
IF EXIST ..\..\..\..\..\..\ambulant\bin\win32\libamplugin_ffmpeg.dll copy ..\..\..\..\..\..\ambulant\bin\win32\libamplugin_ffmpeg.dll ..\..\bin\libamplugin_ffmpeg.dll /y
