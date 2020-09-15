#--------------------------------------------------------------------------
# shader project
#--------------------------------------------------------------------------

PROJECT(shader)
FILE(GLOB shader_headers code/*.h)
FILE(GLOB shader_sources code/*.cc)

SET(files_shader ${shader_headers} ${shader_sources})
SOURCE_GROUP("shader" FILES ${files_shader})

ADD_EXECUTABLE(shader ${files_shader})
TARGET_LINK_LIBRARIES(shader core render)
ADD_DEPENDENCIES(shader core render)