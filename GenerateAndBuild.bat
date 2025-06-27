@echo off
call cmake -S . -B BUILD
echo Generate Successful
call cmake --build BUILD
echo Build Successful

@REM echo Joining WPF project into Cpp solution
@REM call AddProject.bat

echo Building WPF app
call BuildWPF.bat

echo Preparing WPF solution
call python .\PrepareWPFSln.py ".\viz\GCviz\GCviz.sln" ".\BUILD\api\API.vcxproj" ".\BUILD\src\GarbageCollector.vcxproj"

pause
