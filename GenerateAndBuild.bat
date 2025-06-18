@echo off
call cmake -S . -B BUILD
echo Generate Successful
call cmake --build BUILD
echo Build Successful
pause
