include_directories(
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Physics
	${SKYLICHT_ENGINE_PROJECT_DIR}/Irrlicht/Include
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/System
	${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Engine
	${SKYLICHT_ENGINE_PROJECT_DIR}/Bullet3/src
)

if (BUILD_BULLET_PHYSIC_LIB)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Bullet3/src)
endif()

if (BUILD_SKYLICHT_COMPONENTS)
include_directories(${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Components)
endif()

file(GLOB_RECURSE skylicht_physics 
	./*.cpp 
	./*.hpp 
	./*.c 
	./*.h)
setup_project_group("${skylicht_physics}" ${CMAKE_CURRENT_SOURCE_DIR})

add_library(Physics ${ENGINE_SHARED_OR_STATIC_LIB}
	${skylicht_physics}
)

if (BUILD_SHARED_LIBS)
set_target_properties(Physics PROPERTIES WINDOWS_EXPORT_ALL_SYMBOLS true)
endif()

target_precompiled_header(Physics ./pch.cpp ${skylicht_physics})

set_target_properties(Physics PROPERTIES VERSION ${SKYLICHT_VERSION})

target_link_libraries(Physics PRIVATE Engine System)

if (BUILD_SKYLICHT_COMPONENTS)
target_link_libraries(Physics PRIVATE Components)
endif()

if (BUILD_BULLET_PHYSIC_LIB)
target_link_libraries(Physics PRIVATE LinearMath Bullet3Common BulletCollision BulletDynamics BulletInverseDynamics BulletSoftBody)

IF(BUILD_BULLET3)
target_link_libraries(Physics PRIVATE Bullet3Collision Bullet3Dynamics Bullet3Geometry Bullet3OpenCL Bullet2FileLoader)
ENDIF(BUILD_BULLET3)

endif()

if (INSTALL_LIBS)
install(TARGETS Physics
			EXPORT PhysicsTargets
			RUNTIME DESTINATION ${SKYLICHT_RUNTIME_INSTALL_DIR}
			LIBRARY DESTINATION ${SKYLICHT_LIBRARY_INSTALL_DIR}
			ARCHIVE DESTINATION ${SKYLICHT_ARCHIVE_INSTALL_DIR})

install (DIRECTORY ${SKYLICHT_ENGINE_PROJECT_DIR}/Skylicht/Physics
	DESTINATION ${SKYLICHT_INCLUDE_INSTALL_DIR}/Skylicht
	FILES_MATCHING PATTERN "*.h*")

install(EXPORT PhysicsTargets
	FILE PhysicsTargets.cmake
	NAMESPACE Skylicht::
	DESTINATION ${SKYLICHT_TARGET_INSTALL_DIR}
)
endif()