cmake_minimum_required(VERSION 3.20)
project(LearnOpenGL VERSION 0.1.0)

# 生成 compile_commands.json 供 IDE 使用
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# 使用 C++ 17 标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# 添加头文件目录
set(HEADER_DIR ${PROJECT_SOURCE_DIR}/include/)
set(LIB_DIR ${PROJECT_SOURCE_DIR}/lib/)
include_directories(${HEADER_DIR})

# 添加 GLFW 库
set(GLFW_LINK ${LIB_DIR}libglfw.3.dylib)

# 链接 OpenGL 框架（macOS）
if (APPLE)
    set(OPENGL_LIBS "-framework OpenGL")
endif()

# 共用的 glad.c
set(GLAD_SOURCE ${PROJECT_SOURCE_DIR}/src/glad.c)

# 自动扫描所有 day 目录，为每个目录生成一个可执行文件
# 使用 CONFIGURE_DEPENDS：当新增/删除 day 目录或源文件时，CMake 会自动重新配置
file(GLOB DAY_DIRS LIST_DIRECTORIES true CONFIGURE_DEPENDS "${PROJECT_SOURCE_DIR}/src/day*")
foreach(DAY_DIR ${DAY_DIRS})
    # 获取目录名（如 day01, day02）
    get_filename_component(DAY_NAME ${DAY_DIR} NAME)
    
    # 收集该目录下的源文件（仅 .cpp；glad.c 统一由根目录提供）
    # 显式 LIST_DIRECTORIES false：只收集文件，避免行为差异
    file(GLOB_RECURSE DAY_SOURCES
     LIST_DIRECTORIES false
     CONFIGURE_DEPENDS
     "${DAY_DIR}/*.cpp")
    
    # 如果目录下有源文件，创建可执行文件
    if(DAY_SOURCES)
        add_executable(${DAY_NAME} ${DAY_SOURCES} ${GLAD_SOURCE})
        target_link_libraries(${DAY_NAME} ${GLFW_LINK} ${OPENGL_LIBS})

        # 工程化：如果该 day 有 shaders/ 目录，则自动同步到 build/shaders/<dayXX>/
        # 关键点：用 “内容 hash” 判断是否需要同步；并把同步 target 设为 ALL
        # 这样即使可执行文件本身不需要重编译，单纯改了 shader，跑 `make` 也会刷新 build 里的 shader
        set(DAY_SHADERS_DIR "${DAY_DIR}/shaders")
        if (EXISTS "${DAY_SHADERS_DIR}")
            set(DAY_SHADER_OUT_DIR "${CMAKE_BINARY_DIR}/shaders/${DAY_NAME}")
            set(DAY_SHADER_STAMP "${CMAKE_BINARY_DIR}/shaders/${DAY_NAME}.hash")
            set(SYNC_SCRIPT "${PROJECT_SOURCE_DIR}/cmake/sync_shaders.cmake")

            add_custom_target(${DAY_NAME}_shaders ALL
                COMMAND "${CMAKE_COMMAND}"
                    -DSRC_DIR:PATH=${DAY_SHADERS_DIR}
                    -DOUT_DIR:PATH=${DAY_SHADER_OUT_DIR}
                    -DSTAMP_FILE:FILEPATH=${DAY_SHADER_STAMP}
                    -P "${SYNC_SCRIPT}"
                COMMENT "Sync shaders (hash) for ${DAY_NAME}"
                VERBATIM
            )

            # 确保构建 ${DAY_NAME} 时也会先同步一次（虽然 ALL 已经会跑，这里再保险）
            add_dependencies(${DAY_NAME} ${DAY_NAME}_shaders)
        endif()

        # 工程化：同步 day 目录下的图片等资源到 build/assets/<dayXX>/
        # 约定：运行时从 build 目录启动，可用相对路径 "assets/<dayXX>/xxx.jpg"
        file(GLOB DAY_ASSET_FILES LIST_DIRECTORIES false CONFIGURE_DEPENDS
            "${DAY_DIR}/*.png"
            "${DAY_DIR}/*.jpg"
            "${DAY_DIR}/*.jpeg"
        )
        if (DAY_ASSET_FILES)
            set(DAY_ASSET_OUT_DIR "${CMAKE_BINARY_DIR}/assets/${DAY_NAME}")
            add_custom_target(${DAY_NAME}_assets ALL
                COMMAND "${CMAKE_COMMAND}" -E make_directory "${DAY_ASSET_OUT_DIR}"
                COMMENT "Sync assets for ${DAY_NAME}"
                VERBATIM
            )
            foreach(ASSET_FILE ${DAY_ASSET_FILES})
                add_custom_command(TARGET ${DAY_NAME}_assets PRE_BUILD
                    COMMAND "${CMAKE_COMMAND}" -E copy_if_different
                        "${ASSET_FILE}" "${DAY_ASSET_OUT_DIR}/"
                    VERBATIM
                )
            endforeach()
            add_dependencies(${DAY_NAME} ${DAY_NAME}_assets)
        endif()
    endif()
endforeach()

include(CTest)
enable_testing()

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
