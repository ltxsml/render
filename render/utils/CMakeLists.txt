project(utils)

FILE(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS src/*.cpp inc/*.h)
add_library(${PROJECT_NAME} STATIC ${ALL_SRC})
target_include_directories(${PROJECT_NAME} PUBLIC inc)