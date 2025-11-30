set(TARGET_NAME SkyboltWidgetsExampleApp)
add_source_group_tree(. SrcFiles)

include_directories("./")
include_directories("../")

add_executable(${TARGET_NAME} ${SrcFiles})

target_link_libraries (${TARGET_NAME} SkyboltWidgets)
