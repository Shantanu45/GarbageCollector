@echo off
REM Path to your existing C++ solution (.sln)
set SOLUTION_PATH="BUILD\GarbageCollector.sln"

REM Path to your existing C# project (.csproj)
set CSHARP_PROJECT_PATH="viz\GCviz\GCviz\GCviz.csproj"

REM Add the C# project to the solution using dotnet CLI
dotnet sln %SOLUTION_PATH% add %CSHARP_PROJECT_PATH%

echo Successfully added %CSHARP_PROJECT_PATH% to %SOLUTION_PATH%