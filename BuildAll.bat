set PluginPath=MeleeTrace.uplugin
set OutputPath=%1
set BuildVersion=%2
set EnginePath55=%UE_PATH_5_5%
set EnginePath56=%UE_PATH_5_6%
set EnginePath57=%UE_PATH_5_7%

call "%EnginePath55%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.5"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath56%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.6"
if %ERRORLEVEL% GEQ 1 EXIT /B 1
call "%EnginePath57%\Build\BatchFiles\RunUAT.bat" BuildPlugin -plugin="%cd%\%PluginPath%" -package="%OutputPath%\MeleeTrace-%BuildVersion%_Win64_5.7"
if %ERRORLEVEL% GEQ 1 EXIT /B 1