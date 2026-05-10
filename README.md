# GarbageCollector

Study project implementing several garbage collection techniques in C++.

The project models a small virtual heap, allocates objects into it, tracks object
headers and pointer values, and runs different garbage collectors over the
allocated graph. It also includes unit tests and a WPF visualizer that reads GC
and heap statistics through a small C API DLL.

### WPF GC vizualization screenshot
<img width="1576" height="897" alt="image" src="https://github.com/user-attachments/assets/b4dbc02a-a8b5-4b47-b05b-6fbb426395dc" />

## Implemented Components

- Virtual byte-addressed heap
- Tagged values for numbers, booleans, and heap pointers
- Object headers with size, mark/reference metadata, and forwarding addresses
- Free-list allocator
- Mark-sweep collector
- Mark-compact collector
- Copying collector
- GC and heap statistics
- GoogleTest test suite
- WPF visualization project

## Requirements

- Windows
- Visual Studio 2022 with the Desktop development with C++ workload
- CMake
- .NET SDK with Windows/WPF support
- Git, for fetching CMake dependencies such as spdlog

## Build

Generate and build with Visual Studio 2022:

```bat
GenerateAndBuild.bat
```

Or run CMake manually:

```bat
cmake -S . -B BUILD_VS -G "Visual Studio 17 2022" -A x64
cmake --build BUILD_VS --config Debug
```

The Visual Studio solution is generated at:

```text
BUILD_VS\GarbageCollector.sln
```

Main build outputs are written to:

```text
x64\Debug\
```

## Tests

After building, run:

```bat
ctest --test-dir BUILD_VS -C Debug --output-on-failure
```

## Sample

The console sample is in:

```text
src\Sample\main.cpp
```

It currently runs one of the examples from `src\Examples.h`, allocates a small
object graph, performs collection, and prints heap/GC statistics.
