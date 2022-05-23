cmake_minimum_required (VERSION 3.18)

set(PROJ_NAME DependenciesAnalyser)
set(LOAD_TEST_DATA true)

project(${PROJ_NAME} VERSION 0.1.0)

# =======================================================#
# Source Files Settings
# =======================================================#
list(APPEND SOURCE_FILES
    src/main.cpp
    src/Analyser.cpp
    src/Utils.cpp)

add_executable(${PROJ_NAME} ${SOURCE_FILES})

set_target_properties(${PROJ_NAME} PROPERTIES
    CXX_STANDARD 17
    OUTPUT_NAME dinclude
)

if (MSVC) 
    set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_STARTUP_PROJECT ${PROJ_NAME})
endif()

# =======================================================#
# Dependencies
# =======================================================#

if (LOAD_TEST_DATA)
    set(TEST_DATA_NAME testData)
    set(TEST_DATA_DIR  ${CMAKE_CURRENT_BINARY_DIR}/${TEST_DATA_NAME})
    set(TEST_DATA_SOURCE_DIR ${PROJECT_SOURCE_DIR}/${TEST_DATA_NAME}/complex)
    add_custom_target(copy_assets
        COMMAND ${CMAKE_COMMAND} -E rm -rf ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E make_directory ${TEST_DATA_DIR}
        COMMAND ${CMAKE_COMMAND} -E copy_directory ${TEST_DATA_SOURCE_DIR} ${TEST_DATA_DIR}
    )
    add_dependencies(${PROJ_NAME} copy_assets)
endif()

# =======================================================#
# Compiler Settings
# =======================================================#

target_compile_options(${PROJ_NAME} PRIVATE
    $<$<CXX_COMPILER_ID:MSVC>:-Zc:__cplusplus -W4 -wd5045 -analyze>
    $<$<NOT:$<CXX_COMPILER_ID:MSVC>>:-Wall -Wno-c++98-compat -Winline>
)