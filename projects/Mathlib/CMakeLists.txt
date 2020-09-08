#--------------------------------------------------------------------------
# Mathlib project
#--------------------------------------------------------------------------

PROJECT(Mathlib)
FILE(GLOB example_headers code/*.h)
FILE(GLOB example_sources code/*.cc)
FILE(GLOB example_sources code/*.cpp)

SET(files_Mathlib ${example_headers} ${example_sources})
SOURCE_GROUP("Mathlib" FILES ${files_example})

ADD_EXECUTABLE(Mathlib ${files_example})
TARGET_LINK_LIBRARIES(Mathlib core render)
ADD_DEPENDENCIES(Mathlib core render)