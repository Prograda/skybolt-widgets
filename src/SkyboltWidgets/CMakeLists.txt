set(TARGET_NAME SkyboltWidgets)
add_source_group_tree(. SrcFiles)

include_directories("./")
include_directories("../")

# Qt
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_AUTOMOC ON)
find_package(Qt6 COMPONENTS Core Gui Widgets REQUIRED)

if(BUILD_WITH_SKYBOLT_REFLECT)
	find_package(skybolt-reflect REQUIRED)
endif()

SET(SrcFiles ${SrcFiles} ${SrcUiHeaders} ${SrcRccsSrc})
SOURCE_GROUP("Generated Ui Files" FILES ${SrcUiHeaders} ${SrcRccsSrc})

set(LIBS
	qt::qt
)

if(BUILD_WITH_SKYBOLT_REFLECT)
	list(APPEND LIBS skybolt-reflect::skybolt-reflect)
endif()

add_library(${TARGET_NAME} STATIC ${SrcFiles})

target_link_libraries (${TARGET_NAME} ${LIBS})

# Install conan files (required by 'conan editable' feature. See https://docs.conan.io/en/latest/developing_packages/editable_packages.html)
if (EXISTS ${CMAKE_SOURCE_DIR}/conanfile.py) # File might not exist during conan build. See https://github.com/conan-io/conan/issues/5415.
	install(FILES ${CMAKE_SOURCE_DIR}/conanfile.py DESTINATION .)
endif()

# Install Qt dependencies
install(TARGETS SkyboltWidgets
	EXPORT SkyboltWidgetsTargets
	RUNTIME_DEPENDENCY_SET SkyboltWidgetsDependencies
	LIBRARY DESTINATION lib
	ARCHIVE DESTINATION lib
	RUNTIME DESTINATION bin
)