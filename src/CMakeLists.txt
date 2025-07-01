set(PROJECT_NAME GarbageCollector)

set(UNGROUPED_FILES
	Examples.h
)

source_group("" FILES ${UNGROUPED_FILES})

set(MemoryManager
	"MemoryManager/Heap.h"
	"MemoryManager/ObjectHeader.h"
	"MemoryManager/MemoryManager.cpp"
	"MemoryManager/MemoryManager.h"
)
source_group("MemoryManager" FILES ${MemoryManager})

set(FreeList
	"Allocators/FreeList/FreeList.cpp"
	"Allocators/FreeList/FreeList.h"
)
source_group("Allocators/FreeList" FILES ${FreeList})

set(Allocators
	"Allocators/IAllocator.h"
)
source_group("Allocators" FILES ${Allocators})

set(Collectors
	"Collectors/ICollector.h"
	"Collectors/GCHelpers.h"
)
source_group("Collectors" FILES ${Collectors})

set(MarkSweep
	"Collectors/MarkSweepGC/MarkSweepGC.h"
	"Collectors/MarkSweepGC/MarkSweepGC.cpp"
)
source_group("Collectors/MarkSweep" FILES ${MarkSweep})

set(MarkCompact
	"Collectors/MarkCompactGC/MarkCompactGC.h"
	"Collectors/MarkCompactGC/MarkCompactGC.cpp"
)
source_group("Collectors/MarkCompact" FILES ${MarkCompact})

set(CopyingGC
	"Collectors/CopyingGC/CopyingGC.h"
	"Collectors/CopyingGC/CopyingGC.cpp"
)
source_group("Collectors/CopyingGC" FILES ${CopyingGC})

set(Value
	"Value/Value.cpp"
	"Value/Value.h"
)
source_group("Value" FILES ${Value})

set(Stats
	"Stats/GCStats.h"
	"Stats/HeapStats.h"
)
source_group("Stats" FILES ${Stats})

set(Utils
	"Utils/number-util.h"
	"Utils/alloc-util.h"
	"Utils/alloc_util.cpp"
	"Utils/stat-util.h"
	"Utils/log-util.h"
	"Utils/common.h"
	"Utils/timer-util.h"
)
source_group("Utils" FILES ${Utils})

set(ALL_FILES
	${UNGROUPED_FILES}
	${MemoryManager}
	${Allocators}
	${FreeList}
	${Value}
	${Utils}
	${Collectors}
	${MarkSweep}
	${MarkCompact}
	${CopyingGC}
	${Stats}
)


add_library(${PROJECT_NAME}
${ALL_FILES}
)

use_props(${PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")

################################################################################
# Compile definitions
################################################################################


################################################################################
# Compile and link options
################################################################################
# Using target_compile_options in CMake allows you to add specific compiler options to a target, giving you more fine-grained control over how each target is built. 
# This can be useful if you need to pass additional flags to the compiler or enable certain warnings or optimizations.
if(MSVC)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_options(${PROJECT_NAME} PRIVATE
			$<$<CONFIG:Debug>:
				/Od
			>
			$<$<CONFIG:Release>:
				/Oi;
				/Gy
			>
			/permissive-;
			/sdl;
			/W3;
			${DEFAULT_CXX_DEBUG_INFORMATION_FORMAT};
			${DEFAULT_CXX_EXCEPTION_HANDLING}
		)
	endif()
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_link_options(${PROJECT_NAME} PRIVATE
			$<$<CONFIG:Release>:
				/OPT:REF;
				/OPT:ICF
			>
			/DEBUG;
			/SUBSYSTEM:CONSOLE
		)
	endif()
endif()

set(INCLUDES 
"${CMAKE_BINARY_DIR}/_deps/googletest-src/googletest/include"
)

target_include_directories(${PROJECT_NAME} PRIVATE ${INCLUDES})

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	spdlog::spdlog
)

target_link_libraries(${PROJECT_NAME} PRIVATE ${ADDITIONAL_LIBRARY_DEPENDENCIES})

target_compile_options(spdlog PRIVATE /EHsc)