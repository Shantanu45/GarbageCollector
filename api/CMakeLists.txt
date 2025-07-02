set(GCAPI_PROJECT API)

set(ADDITIONAL_LIBRARY_DEPENDENCIES
	GarbageCollector
	spdlog::spdlog
)

set(INCLUDES 
"${PROJECT_SOURCE_DIR}/src"
)

# DLL requires position independent code
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

set(UNGROUPED_FILES
	api.cpp
	api.h
)
source_group("" FILES ${UNGROUPED_FILES})

set(ALL_FILES
	${UNGROUPED_FILES}
)


add_library(${GCAPI_PROJECT} SHARED
${ALL_FILES}
)

use_props(${GCAPI_PROJECT} "${CMAKE_CONFIGURATION_TYPES}" "${DEFAULT_CXX_PROPS}")

if(MSVC)
	if("${CMAKE_VS_PLATFORM_NAME}" STREQUAL "x64")
		target_compile_options(${GCAPI_PROJECT} PRIVATE
			$<$<OR:$<CONFIG:Debug>,$<CONFIG:DebugHeadless>>:
				/Od
			>
			$<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseHeadless>>:
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
		target_link_options(${GCAPI_PROJECT} PRIVATE
			$<$<OR:$<CONFIG:Release>,$<CONFIG:ReleaseHeadless>>:
				/OPT:REF;
				/OPT:ICF
			>
			/DEBUG;
			/SUBSYSTEM:CONSOLE
		)
	endif()
endif()

target_compile_definitions(${GCAPI_PROJECT} PRIVATE GCAPI_EXPORTS)

target_link_libraries(${GCAPI_PROJECT} PRIVATE "${ADDITIONAL_LIBRARY_DEPENDENCIES}")
target_include_directories(${GCAPI_PROJECT} PRIVATE ${INCLUDES})