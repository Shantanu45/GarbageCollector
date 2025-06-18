set(PROJECT_NAME GarbageCollector)

set(UNGROUPED_FILES
	"main.cpp"
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

set(Value
	"Value/Value.cpp"
	"Value/Value.h"
)
source_group("Value" FILES ${Value})

set(Utils
	"Utils/number-util.h"
)
source_group("Utils" FILES ${Utils})

set(ALL_FILES
	${UNGROUPED_FILES}
	${MemoryManager}
	${Allocators}
	${FreeList}
	${Value}
	${Utils}
)


add_executable(${PROJECT_NAME}
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
				/std:c++20;
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
