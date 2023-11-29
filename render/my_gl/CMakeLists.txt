project(my_gl)

FILE(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS *.cpp *.h)
add_library(${PROJECT_NAME} STATIC ${ALL_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC .)
target_link_libraries(${PROJECT_NAME} PUBLIC utils)
