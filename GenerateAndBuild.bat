@echo off
call cmake -S . -B BUILD
echo Generate Successful
call cmake --build BUILD
echo Build Successful

echo Joining WPF project into Cpp solution
call AddProject.bat

pause
