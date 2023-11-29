project(utest)

FILE(GLOB_RECURSE ALL_SRC CONFIGURE_DEPENDS *.cpp)

SET(EXECUTABLE_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/../static/exe)

FOREACH (F ${ALL_SRC})
    get_filename_component(TName ${F} NAME_WE)
    add_executable(${TName} ${F})
    target_link_libraries(${TName} PUBLIC my_gl)
ENDFOREACH (F)