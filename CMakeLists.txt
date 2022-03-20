cmake_minimum_required(VERSION 3.22)

project(voxels)

# Find Vulkan SDK, requiring glslc compiler component.
find_package(Vulkan REQUIRED COMPONENTS glslc)

function(compile_shaders_to_spirv GLSL_DIRECTORY SPIRV_DIRECTORY GLSLC_OPTIONS GLSL_FILES)

    # Make sure the output directory actually exists
    file(MAKE_DIRECTORY ${SPIRV_DIRECTORY})

    # Add commands for compiling all glsl files
    foreach(GLSL_FILE ${GLSL_FILES})

        add_custom_command(
            OUTPUT ${SPIRV_DIRECTORY}/${GLSL_FILE}.spv
            DEPENDS ${GLSL_DIRECTORY}/${GLSL_FILE}
            COMMAND ${Vulkan_GLSLC_EXECUTABLE} ${GLSL_DIRECTORY}/${GLSL_FILE} ${GLSLC_OPTIONS} ${SPIRV_DIRECTORY}/${GLSL_FILE}.spv
        )

        # Add compiled files to list so a target depending on them can be created 
        list(APPEND SPIRV_FILES ${SPIRV_DIRECTORY}/${GLSL_FILE}.spv)

    endforeach()

    # Create target depending on creation of all spirv files
    add_custom_target(SPIRV_SHADERS ALL DEPENDS ${SPIRV_FILES})

endfunction()

set(GLSL_FILES trace.comp init_checkempty.comp init_assignindex.comp init_fillbricks.comp)

set(GLSLC_OPTIONS -O --target-env=vulkan1.1 -o)

set(OCH_LIB_DIR C:/Users/alex_2/source/repos/och_lib/och_lib)

set(OCH_LIB_HEADERS 
    ${OCH_LIB_DIR}/och_time.h
    ${OCH_LIB_DIR}/och_err.h
    ${OCH_LIB_DIR}/och_fmt.h
    ${OCH_LIB_DIR}/och_fio.h
    ${OCH_LIB_DIR}/och_utf8.h
    ${OCH_LIB_DIR}/och_timer.h
    ${OCH_LIB_DIR}/och_matmath.h
    ${OCH_LIB_DIR}/och_range.h
    ${OCH_LIB_DIR}/och_type_union.h
    ${OCH_LIB_DIR}/och_virtual_keys.h
)

set(OCH_LIB_SOURCES 
    ${OCH_LIB_DIR}/och_time.cpp
    ${OCH_LIB_DIR}/och_err.cpp
    ${OCH_LIB_DIR}/och_fmt.cpp
    ${OCH_LIB_DIR}/och_fio.cpp
    ${OCH_LIB_DIR}/och_utf8.cpp
)

add_executable(
    ${PROJECT_NAME}
    main.cpp
    vulkan_base.cpp
    voxel_volume.cpp
    vulkan_base.hpp
    voxel_volume.hpp
    ${Vulkan_INCLUDE_DIR}
    ${OCH_LIB_SOURCES}
    ${OCH_LIB_HEADERS})

target_include_directories(${PROJECT_NAME} PRIVATE ${Vulkan_INCLUDE_DIR} ${OCH_LIB_DIR})

target_link_libraries(${PROJECT_NAME} PRIVATE ${Vulkan_LIBRARIES})

target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_17)

target_compile_definitions(${PROJECT_NAME} PRIVATE OCH_USING_VULKAN OCH_ERROR_CONTEXT_EXTENDED OCH_VALIDATE)

compile_shaders_to_spirv(${PROJECT_SOURCE_DIR}/shaders ${PROJECT_BINARY_DIR}/spirv "${GLSLC_OPTIONS}" "${GLSL_FILES}")
