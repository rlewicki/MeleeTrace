set PluginPath=MeleeTrace.uplugin
set OutputPath=%1
set BuildVersion=%2
set EnginePath51=%UE_PATH_5_1%
set EnginePath52=%UE_PATH_5_2%
set EnginePath53=%UE_PATH_5_3%
set EnginePath54=%UE_PATH_5_4%
set EnginePath55=%UE_PATH_5_5%

call "%EnginePath51%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.1"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath52%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.2"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath53%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.3"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath54%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.4"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath55%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.5"
if %ERRORLEVEL% GEQ 1 EXIT /B 1