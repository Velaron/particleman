cmake_minimum_required(VERSION 2.8.12)
project(particleman)

set(PMAN_LIB particleman)

file(GLOB_RECURSE PMAN_SRC "particleman/**/*.cpp")

if(NOT MSVC)
	add_definitions(-D_LINUX)
	add_definitions(-Dstricmp=strcasecmp -Dstrnicmp=strncasecmp -D_snprintf=snprintf -D_vsnprintf=vsnprintf)
	add_compile_options(-Wno-switch -Wno-implicit-exception-spec-mismatch -Wno-writable-strings)
endif()

include_directories(
	particleman/src/common
	particleman/src/dlls
	particleman/src/engine
	particleman/src/particleman/particlefx_header
	particleman/src/pm_shared
	particleman/src/public
)

add_library(${PMAN_LIB} MODULE ${PMAN_SRC})

#[[
install(TARGETS ${PMAN_LIB}
	DESTINATION "${GAMEDIR}/${CLIENT_INSTALL_DIR}/"
	PERMISSIONS OWNER_READ OWNER_WRITE OWNER_EXECUTE
		GROUP_READ GROUP_EXECUTE
		WORLD_READ WORLD_EXECUTE)

# Install PDB file on Windows
if(WIN32 AND MSVC)
	install(FILES $<TARGET_PDB_FILE:${PMAN_LIB}>
		DESTINATION "${GAMEDIR}/${CLIENT_INSTALL_DIR}/" OPTIONAL)
endif()
]]