set(APP_NAME SkyboltWidgetsTests)

file(GLOB SOURCE_FILES *.cpp *.h)

include_directories("../")
include_directories("../../Skybolt")

find_package(Catch2)

add_executable(${APP_NAME} ${SOURCE_FILES})

target_link_libraries (${APP_NAME} SkyboltWidgets Catch2::Catch2)

catch_discover_tests(${APP_NAME})