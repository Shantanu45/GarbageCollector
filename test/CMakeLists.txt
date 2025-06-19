set(TEST_PROJECT_NAME Test)

set(UNGROUPED_FILES
	"main.cpp"
)

source_group("" FILES ${UNGROUPED_FILES})

set(TESTS
	"FreeListAllocatorTest.cpp"
	"HeapTest.cpp"
	"MemoryManagerTest.cpp"
	"ObjectHeaderTest.cpp"
	"ValueTest.cpp"
)

source_group("tests" FILES ${TESTS})

include(FetchContent)
FetchContent_Declare(
  googletest
  URL https://github.com/google/googletest/archive/52eb8108c5bdec04579160ae17225d66034bd723.zip
)
# For Windows: Prevent overriding the parent project's compiler/linker settings
set(gtest_force_shared_crt ON CACHE BOOL "" FORCE)
FetchContent_MakeAvailable(googletest)

if(MSVC)
  set_target_properties(gtest         PROPERTIES FOLDER "ThirdParty/GTest")
  set_target_properties(gtest_main    PROPERTIES FOLDER "ThirdParty/GTest")
  set_target_properties(gmock         PROPERTIES FOLDER "ThirdParty/GTest")
  set_target_properties(gmock_main    PROPERTIES FOLDER "ThirdParty/GTest")
endif()


set(ALL_FILES
	${UNGROUPED_FILES}
	${TESTS}
	${PROJECT_SOURCE_DIR}/src/Value/Value.cpp
	${PROJECT_SOURCE_DIR}/src/Allocators/FreeList/FreeList.cpp
)

add_executable(${TEST_PROJECT_NAME}
	${ALL_FILES}
)

set(GC_INCLUDE "${PROJECT_SOURCE_DIR}/src")

target_include_directories(${TEST_PROJECT_NAME} PRIVATE ${GC_INCLUDE})

use_props(${TEST_PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")

################################################################################
# Compile and link options
################################################################################
# Using target_compile_options in CMake allows you to add specific compiler options to a target, giving you more fine-grained control over how each target is built. 
# This can be useful if you need to pass additional flags to the compiler or enable certain warnings or optimizations.
if(MSVC)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_options(${TEST_PROJECT_NAME} PRIVATE
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
		target_link_options(${TEST_PROJECT_NAME} PRIVATE
			$<$<CONFIG:Release>:
				/OPT:REF;
				/OPT:ICF
			>
			/DEBUG;
			/SUBSYSTEM:CONSOLE
		)
	endif()
endif()

# Link with GTest
target_link_libraries(Test gtest_main)

# Add test to CTest
include(GoogleTest)
gtest_discover_tests(Test)
