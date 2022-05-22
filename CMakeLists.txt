cmake_minimum_required (VERSION 3.15)

set(PROJ_NAME DependenciesAnalyser)
project(${PROJ_NAME})

# =======================================================#
# Source Files Settings
# =======================================================#
list(APPEND SOURCE_FILES
    src/main.cpp
    src/Analyser.cpp
    src/Utils.cpp
)

add_executable(${PROJ_NAME} ${SOURCE_FILES})

set_property(TARGET ${PROJ_NAME} PROPERTY CXX_STANDARD 17)
if (MSVC) 
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})
endif()

# =======================================================#
# Compiler Settings
# =======================================================#

target_compile_options(${PROJ_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-Zc:__cplusplus -W4 -wd5045 -analyze>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wno-c++98-compat -Winline>
)