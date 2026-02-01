set(APP_NAME SkyboltWidgetsTests)

file(GLOB SOURCE_FILES *.cpp *.h)

include_directories("../")

find_package(Catch2 REQUIRED)

add_executable(${APP_NAME} ${SOURCE_FILES})

target_link_libraries (${APP_NAME} SkyboltWidgets Catch2::Catch2)

catch_discover_tests(${APP_NAME})