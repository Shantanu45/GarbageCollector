set(SAMPLE_PROJECT_NAME SampleGC)

set(UNGROUPED_FILES
	main.cpp
)
source_group("" FILES ${UNGROUPED_FILES})

set(ALL_FILES
	${UNGROUPED_FILES}
)

add_executable(${SAMPLE_PROJECT_NAME}
	${ALL_FILES}
)

use_props(${SAMPLE_PROJECT_NAME} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")

if(MSVC)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_options(${SAMPLE_PROJECT_NAME} PRIVATE
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
		target_link_options(${SAMPLE_PROJECT_NAME} PRIVATE
			$<$<CONFIG:Release>:
				/OPT:REF;
				/OPT:ICF
			>
			/DEBUG;
			/SUBSYSTEM:CONSOLE
		)
	endif()
endif()

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	GarbageCollector
	spdlog::spdlog
)


target_link_libraries(${SAMPLE_PROJECT_NAME} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")

set(INCLUDES 
"${PROJECT_SOURCE_DIR}/src"
"${CMAKE_BINARY_DIR}/_deps/googletest-src/googletest/include"
)

target_compile_definitions(${SAMPLE_PROJECT_NAME} PRIVATE CPP_CONSOLE_CLIENT)

target_include_directories(${SAMPLE_PROJECT_NAME} PRIVATE ${INCLUDES})