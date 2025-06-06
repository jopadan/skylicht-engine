include_directories(
	${SKYLICHT_ENGINE_SOURCE_DIR}/Samples/Audio/Source
	${SKYLICHT_ENGINE_PROJECT_DIR}/Main
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Client
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty
	${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdParty/freetype2/include
)

if (BUILD_SKYLICHT_AUDIO)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Audio)
endif()

if (BUILD_SKYLICHT_LIGHMAPPER)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Lightmapper)
endif()

if (BUILD_SKYLICHT_COMPONENTS)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components)
endif()

if (BUILD_SKYLICHT_COLLISION)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Collision)
endif()

if (BUILD_IMGUI)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Imgui)
endif()

if (BUILD_SKYLICHT_PHYSIC)
	include_directories(
		${SKYLICHT_ENGINE_PROJECT_DIR}/Bullet3/src
		${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Physics
	)
endif()

if (BUILD_SPINE_RUNTIMES)
	include_directories(
		${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-cpp/include
		${SKYLICHT_ENGINE_PROJECT_DIR}/SpineCpp/spine-runtimes
	)
endif()

if (BUILD_SKYLICHT_UI)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/UserInterface)
endif()

if (BUILD_SKYLICHT_NETWORK)
	include_directories(
		${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Crypto
		${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Network
	)
endif()

if (BUILD_SKYLICHT_GRAPH)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Graph)
endif()

set(template_path ${SKYLICHT_ENGINE_PROJECT_DIR}/Main)

file(GLOB_RECURSE application_source 
	./Source/**.cpp 
	./Source/**.c 
	./Source/**.h)

# Custom additional sources
include(CMakeProject.cmake)

setup_project_group("${application_source}" ${CMAKE_CURRENT_SOURCE_DIR})

list (APPEND application_source ${template_path}/pch.cpp)
list (APPEND application_source ${template_path}/pch.h)

if (BUILD_ANDROID)
	file(GLOB_RECURSE platform_android_source 
		${template_path}/Platforms/Android/**.cpp 
		${template_path}/Platforms/Android/**.c 
		${template_path}/Platforms/Android/**.h)

	list (APPEND application_source ${platform_android_source})
endif()

if (BUILD_WINDOWS_STORE)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio)
	file(MAKE_DIRECTORY ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio/Assets)
	
	file(GLOB uwp_assets 
		${template_path}/Platforms/UWP/Assets/**.*
		${SKYLICHT_ENGINE_BIN_DIR}/**.zip)
	
	foreach(asset_file ${uwp_assets})
		file(COPY "${asset_file}" DESTINATION ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio/Assets)
	endforeach()

	file(COPY ${template_path}/Platforms/UWP/Package.appxmanifest
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio)
		
	file(COPY ${template_path}/Platforms/UWP/TemporaryKey.pfx
		DESTINATION 
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio)

	file(GLOB_RECURSE platform_uwp_source 
		${template_path}/Platforms/UWP/**.cpp 
		${template_path}/Platforms/UWP/**.c 
		${template_path}/Platforms/UWP/**.h)
	
	# copy resource to generate prj folder
	file(GLOB_RECURSE platform_uwp_asset ${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio/Assets/**.*)
	add_source_group("${platform_uwp_asset}" "Assets")
	
	list (APPEND application_source ${platform_uwp_source})

	set_property(SOURCE ${platform_uwp_asset} PROPERTY VS_DEPLOYMENT_CONTENT 1)
endif()

if (MSVC OR CYGWIN OR MINGW)
	include_directories(${template_path}/Platforms/Win32)

	file(GLOB_RECURSE platform_win32_source 
		${template_path}/Platforms/Win32/**.cpp 
		${template_path}/Platforms/Win32/**.c 
		${template_path}/Platforms/Win32/**.h)

	list (APPEND application_source ${platform_win32_source})
endif()

if (BUILD_MACOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
	include_directories(${template_path}/Platforms/MacOS)
	
	file(GLOB_RECURSE platform_mac_source 
		${template_path}/Platforms/MacOS/**.cpp 
		${template_path}/Platforms/MacOS/**.c 
		${template_path}/Platforms/MacOS/**.h
		${template_path}/Platforms/MacOS/**.m
		${template_path}/Platforms/MacOS/**.mm)

	list (APPEND application_source ${platform_mac_source})
endif()

if (BUILD_IOS)
	# Angle API
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/include)
	include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Source/Angle)
	include_directories(${template_path}/Platforms/IOS)
	
	file(GLOB_RECURSE platform_ios_source 
		${template_path}/Platforms/IOS/**.cpp 
		${template_path}/Platforms/IOS/**.c 
		${template_path}/Platforms/IOS/**.h
		${template_path}/Platforms/IOS/**.m
		${template_path}/Platforms/IOS/**.mm)

	file(GLOB_RECURSE platform_ios_resources ${template_path}/Platforms/IOS/**.storyboard)

	list (APPEND application_source ${platform_ios_source})
endif()

if (BUILD_DEBUG_VLD)
	if (CMAKE_CL_64)
		set(vld_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/bin/Win64/*.*")
	else()
		set(vld_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/Vld/bin/Win32/*.*")
	endif()
endif()
	
if (BUILD_SDL AND MSVC)
	if (CMAKE_CL_64)
		set(sdl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x64")
		set(sdl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x64/*.dll")
	else()
		set(sdl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x86")
		set(sdl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/SDL/lib/x86/*.dll")
	endif()	
	link_directories(${sdl_lib_path})
endif()	


if (BUILD_OPENSSL AND MSVC)
	if (CMAKE_CL_64)
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x64")
		set(openssl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x64/*.dll")
	else()
		set(openssl_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x86")
		set(openssl_dll_path "${SKYLICHT_ENGINE_PROJECT_DIR}/ThirdPartySDK/boringssl/x86/*.dll")
	endif()	
	link_directories(${openssl_lib_path})
endif()

if (MINGW OR CYGWIN)
	# .rc build
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> <FLAGS> -O coff <DEFINES> -i <SOURCE> -o <OBJECT>")    
endif()

if (BUILD_EMSCRIPTEN)
	add_executable(SampleAudio 
		${application_source}
		${template_path}/Platforms/Emscripten/MainWebGL.cpp
	)
elseif(BUILD_SDL)
	add_executable(SampleAudio 
		${application_source}
		${template_path}/Platforms/SDL2/MainSDL.cpp
	)
elseif(BUILD_LINUX)
	add_executable(SampleAudio 
		${application_source}
		${template_path}/Platforms/Linux/MainLinux.cpp
	)
elseif(BUILD_ANDROID)
	add_library(SampleAudio SHARED ${application_source})
elseif (BUILD_WINDOWS_STORE)
	add_executable(SampleAudio
		${application_source}
		${platform_uwp_asset}
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio/TemporaryKey.pfx
		${SKYLICHT_ENGINE_SOURCE_DIR}/PrjUWP/Samples/Audio/Package.appxmanifest
	)
elseif (MSVC OR CYGWIN OR MINGW)
	add_executable(SampleAudio WIN32
		${application_source}
		${template_path}/Platforms/Win32/Skylicht.rc
	)
elseif (BUILD_MACOS)
	file(GLOB resources_files "${SKYLICHT_ENGINE_BIN_DIR}/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/out/MacOS/Release/${CMAKE_OSX_ARCHITECTURES}")
	file(GLOB dylib_files "${angle_lib_path}/*.dylib")

	add_executable(SampleAudio 
		${application_source} 
		${resources_files}
		${dylib_files}
	)
	
	set_target_properties(SampleAudio PROPERTIES MACOSX_BUNDLE TRUE)
	
	target_link_libraries(SampleAudio "-framework Cocoa")
		
	message(STATUS "- Setup project: SampleAudio")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_BIN_DIR}" ${res_file})
		message(STATUS "  - Add resources: ${res_path}")
		set_property(SOURCE ${res_file} PROPERTY MACOSX_PACKAGE_LOCATION "Resources")
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${dylib_files})
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
		source_group("Libs" FILES "${lib_file}")
	endforeach()

	add_custom_command(TARGET SampleAudio 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libEGL.dylib @rpath/libEGL.dylib
		$<TARGET_FILE:SampleAudio>)

	add_custom_command(TARGET SampleAudio 
		POST_BUILD COMMAND 
		${CMAKE_INSTALL_NAME_TOOL} -change ./libGLESv2.dylib @rpath/libGLESv2.dylib
		$<TARGET_FILE:SampleAudio>)

	set_target_properties(SampleAudio PROPERTIES
		MACOSX_RPATH ON
		BUILD_WITH_INSTALL_RPATH 1
		INSTALL_RPATH "@executable_path/../Frameworks"
	)
	
	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH $(LOCAL_APPS_DIR))
elseif (BUILD_IOS)
	set(MACOSX_BUNDLE_EXECUTABLE_NAME SampleAudio)

	set(APP_NAME "SampleAudio")
	set(APP_BUNDLE_IDENTIFIER "com.team_name.project_name")
	set(CODE_SIGN_IDENTITY "iPhone Developer")

	set(PRODUCT_NAME ${APP_NAME})
	set(EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_EXECUTABLE_NAME ${APP_NAME})
	set(MACOSX_BUNDLE_BUNDLE_NAME ${APP_BUNDLE_IDENTIFIER})
	set(MACOSX_BUNDLE_GUI_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})

	file(GLOB resources_files "${SKYLICHT_ENGINE_BIN_DIR}/*.zip")

	set(angle_lib_path "${SKYLICHT_ENGINE_PROJECT_DIR}/Angle/out/IOS/Release/${IOS_PLATFORM}")
	file(GLOB angle_framework_files "${angle_lib_path}/*.framework")

	set (framework_files "")
	list (APPEND framework_files ${angle_framework_files})

	add_executable(SampleAudio MACOSX_BUNDLE 
		${application_source}
		${resources_files}
		${platform_ios_resources}
		${framework_files}
	)
	
	message(STATUS "- Setup project: SampleAudio")
	foreach(res_file ${resources_files})
		file(RELATIVE_PATH res_path "${SKYLICHT_ENGINE_BIN_DIR}" ${res_file})
		source_group("Bin" FILES "${res_file}")
	endforeach()

	foreach(lib_file ${framework_files})
		source_group("Frameworks" FILES "${lib_file}")
		set_property(SOURCE ${lib_file} PROPERTY MACOSX_PACKAGE_LOCATION "Frameworks")
		set_source_files_properties(${lib_file} PROPERTIES XCODE_FILE_ATTRIBUTES "CodeSignOnCopy")
	endforeach()

	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_PRODUCT_BUNDLE_IDENTIFIER ${APP_BUNDLE_IDENTIFIER})
	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_CODE_SIGN_IDENTITY ${CODE_SIGN_IDENTITY})
	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_TARGETED_DEVICE_FAMILY ${DEVICE_FAMILY})
	set_target_properties(SampleAudio PROPERTIES MACOSX_BUNDLE_INFO_PLIST "${template_path}/Platforms/IOS/plist.in")
	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_ASSETCATALOG_COMPILER_APPICON_NAME AppIcon)
	set_target_properties(SampleAudio PROPERTIES XCODE_ATTRIBUTE_INSTALL_PATH $(LOCAL_APPS_DIR))

	# add assets icon
	target_sources(SampleAudio PRIVATE "${template_path}/Platforms/IOS/Assets.xcassets")
	
	list (APPEND platform_ios_resources "${template_path}/Platforms/IOS/Assets.xcassets")
	list (APPEND platform_ios_resources "${resources_files}")
	set_target_properties(SampleAudio PROPERTIES RESOURCE "${platform_ios_resources}")

	# link framework
	target_link_libraries(SampleAudio "-framework Foundation")
	target_link_libraries(SampleAudio "-framework UIKit")
	target_link_libraries(SampleAudio "-framework CoreGraphics")
	target_link_libraries(SampleAudio "-framework Metal")
	target_link_libraries(SampleAudio "-framework MetalKit")
	target_link_libraries(SampleAudio "-framework AudioToolbox")
	target_link_libraries(SampleAudio "-framework AVFAudio")
else()
	add_executable(SampleAudio ${application_source})
endif()

target_precompiled_header(SampleAudio ${template_path}/pch.cpp ${application_source})

# Linker
if (BUILD_ANDROID)
	target_link_libraries(SampleAudio Client log)
else()
	target_link_libraries(SampleAudio Client)
endif()

# Imgui
if (BUILD_IMGUI)
	target_link_libraries(SampleAudio Imgui)
endif()

# Spine runtimes
if (BUILD_SPINE_RUNTIMES)
	target_link_libraries(SampleAudio SpineRuntimes)
endif()

# OpenSSL
if (BUILD_OPENSSL AND MSVC)
	target_link_libraries(SampleAudio crypto.lib decrepit.lib ssl.lib Crypt32.lib)
endif()

# Emscripten
if (BUILD_EMSCRIPTEN)
	message(STATUS "Setting build in data: ${CMAKE_CURRENT_BINARY_DIR}")
	file(COPY "${SKYLICHT_ENGINE_BIN_DIR}/BuiltIn" DESTINATION "${CMAKE_CURRENT_BINARY_DIR}")
	
	message(STATUS "Setting compilation target to WASM")
	set(CMAKE_EXECUTABLE_SUFFIX ".wasm.html")
	set_target_properties(SampleAudio PROPERTIES LINK_FLAGS "-s USE_SDL=2 -s USE_WEBGL2=1 -s FORCE_FILESYSTEM=1 -s FETCH=1 -s ALLOW_MEMORY_GROWTH=1 -s EXPORTED_FUNCTIONS=['_main','_main_resize'] -s EXPORTED_RUNTIME_METHODS=['ccall'] --preload-file ./BuiltIn --shell-file ${template_path}/Platforms/Emscripten/Shell/shell.html --disable-shared -s WASM=1 -s BINARYEN_METHOD='native-wasm'")
	
	set(project_name SampleAudio)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_BIN_DIR}/Index.html)
	configure_file(${template_path}/Platforms/Emscripten/Index.html ${SKYLICHT_ENGINE_BIN_DIR}/SampleAudio.html)
endif()

#VLD
if (BUILD_DEBUG_VLD)
	file(GLOB_RECURSE vld_bin_files ${vld_dll_path})
	foreach(vld_bin ${vld_bin_files})
		add_custom_command(TARGET SampleAudio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${vld_bin} $<TARGET_FILE_DIR:SampleAudio>)
	endforeach()
endif()

#SDL
if (BUILD_SDL AND MSVC)
	file(GLOB_RECURSE sdl_bin_files ${sdl_dll_path})
	foreach(sdl_bin ${sdl_bin_files})
		add_custom_command(TARGET SampleAudio POST_BUILD COMMAND ${CMAKE_COMMAND} -E copy_if_different ${sdl_bin} $<TARGET_FILE_DIR:SampleAudio>)
	endforeach()
endif()

set_target_properties(SampleAudio PROPERTIES VERSION ${SKYLICHT_VERSION})
set_target_properties(SampleAudio PROPERTIES VS_DEBUGGER_WORKING_DIRECTORY "${CMAKE_RUNTIME_OUTPUT_DIRECTORY}")
