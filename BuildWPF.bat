@echo off
call cmake --build BUILD_VS --config Debug --target API
if errorlevel 1 exit /b %errorlevel%

call dotnet build .\viz\GCviz\GCviz\GCviz.csproj -c Debug -p:Platform=x64
if errorlevel 1 exit /b %errorlevel%
