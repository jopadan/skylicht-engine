include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/libmpg123
)

file(GLOB_RECURSE skylicht_audio_source 
	./*.cpp 
	./*.hpp 
	./*.c
	./*.h)
setup_project_group("${skylicht_audio_source}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Audio ${ENGINE_SHARED_OR_STATIC_LIB} ${skylicht_audio_source})

if (BUILD_SHARED_LIBS)
	set_target_properties(Audio PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
	add_definitions(-DAUDIO_EXPORTS)
endif()

target_precompiled_header(Audio ./stdafx.cpp ${skylicht_audio_source})

set_target_properties(Audio PROPERTIES VERSION ${SKYLICHT_VERSION})

if (BUILD_LINUX)
	target_link_libraries(Audio 
		System
		ThirdParty
		openal
	)
elseif(BUILD_ANDROID)
	target_link_libraries(Audio 
		System
		ThirdParty
		OpenSLES
	)
elseif(BUILD_MACOS)
	target_link_libraries(Audio 
		System
		ThirdParty
		"-framework AudioUnit"
		"-framework AVFoundation"
	)
else()
	target_link_libraries(Audio 
		System
		ThirdParty
	)
if (MSVC OR CYGWIN OR MINGW)
	target_link_libraries(Audio Winmm)
endif()
endif()

if (INSTALL_LIBS)
install(TARGETS Audio
			EXPORT AudioTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT AudioTargets
	FILE AudioTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()

