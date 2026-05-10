@echo off
set BUILD_DIR=BUILD_VS
call cmake -S . -B %BUILD_DIR% -G "Visual Studio 17 2022" -A x64
if errorlevel 1 exit /b %errorlevel%
echo Generate Successful
call cmake --build %BUILD_DIR% --config Debug
if errorlevel 1 exit /b %errorlevel%
echo Build Successful

@REM echo Joining WPF project into Cpp solution
@REM call AddProject.bat

@REM echo Building WPF app
@REM call BuildWPF.bat

echo Preparing WPF solution
if not exist ".\%BUILD_DIR%\api\API.vcxproj" goto SkipWPFPrep
if not exist ".\%BUILD_DIR%\src\GarbageCollector.vcxproj" goto SkipWPFPrep
call python .\PrepareWPFSln.py ".\viz\GCviz\GCviz.sln" ".\%BUILD_DIR%\api\API.vcxproj" ".\%BUILD_DIR%\src\GarbageCollector.vcxproj"
if errorlevel 1 exit /b %errorlevel%
goto DoneWPFPrep

:SkipWPFPrep
echo Skipping WPF solution prep: Visual Studio project files were not generated.

:DoneWPFPrep

pause
