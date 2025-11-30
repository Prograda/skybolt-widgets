cmake_minimum_required (VERSION 3.26.0)

project(SkyboltWidgets)

enable_testing()

set_property(GLOBAL PROPERTY USE_FOLDERS ON)

list(APPEND CMAKE_MODULE_PATH
	"${CMAKE_SOURCE_DIR}/cmake/"
	"${CMAKE_BINARY_DIR}"
)

#-------------------------------------------------------------
# Compiler flags
#-------------------------------------------------------------
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

if ( MSVC )
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /we4715") # Treat missing return as error
endif()

#-------------------------------------------------------------
# Dependency options
#-------------------------------------------------------------
OPTION(BUILD_WITH_SKYBOLT_REFLECT "Build with skybolt-reflect enabled" OFF)

if(BUILD_WITH_SKYBOLT_REFLECT)
	add_definitions(-DBUILD_WITH_SKYBOLT_REFLECT)
endif()

#-------------------------------------------------------------
# Build tree
#-------------------------------------------------------------
include(AddSourceGroup)
include(Catch)

add_subdirectory(src/SkyboltWidgets)
add_subdirectory(src/SkyboltWidgetsExampleApp)
add_subdirectory(src/SkyboltWidgetsTests)

#-------------------------------------------------------------
# Installation
#-------------------------------------------------------------
# Install conan files (required by 'conan editable' feature. See https://docs.conan.io/en/latest/developing_packages/editable_packages.html)
if (EXISTS ${CMAKE_SOURCE_DIR}/conanfile.py) # File might not exist if during conan build. See https://github.com/conan-io/conan/issues/5415.
	install(FILES ${CMAKE_SOURCE_DIR}/conanfile.py DESTINATION .)
endif()

# Install CMake scripts
install(DIRECTORY ${CMAKE_SOURCE_DIR}/CMake	DESTINATION .)

# Install conan files (required by 'conan editable' feature. See https://docs.conan.io/en/latest/developing_packages/editable_packages.html)
if (EXISTS ${CMAKE_SOURCE_DIR}/conanfile.py) # File might not exist during conan build. See https://github.com/conan-io/conan/issues/5415.
	install(FILES ${CMAKE_SOURCE_DIR}/conanfile.py DESTINATION .)
endif()

# Install headers
install(
	DIRECTORY ${CMAKE_SOURCE_DIR}/src/SkyboltWidgets
	DESTINATION include
	FILES_MATCHING
	PATTERN "*.h*"
)

# Install runtime dependencies
install(RUNTIME_DEPENDENCY_SET SkyboltDependencies
	PRE_EXCLUDE_REGEXES
		[=[api-ms-]=] # VC Redistibutable DLLs
		[=[ext-ms-]=] # Windows extension DLLs
		[[kernel32\.dll]]
		[[libc\.so\..*]] [[libgcc_s\.so\..*]] [[libm\.so\..*]] [[libstdc\+\+\.so\..*]]
	POST_EXCLUDE_REGEXES
		[=[.*system32\/.*\.dll]=] # Windows system DLLs
		[=[^\/(lib|usr\/lib|usr\/local\/lib)]=] # Unix system libraries
	DIRECTORIES ${CONAN_RUNTIME_LIB_DIRS}
)