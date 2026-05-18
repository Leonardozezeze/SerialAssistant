cmake_minimum_required(VERSION 3.16)
project(SerialAssistant VERSION 1.0.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Qt 需要的自动化工具
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)   # 自动处理 .qrc 资源文件
set(CMAKE_AUTOUIC ON)   # 如果有 .ui 文件则自动处理

# 可执行文件输出目录（统一放在 bin 下）
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

if(WIN32)
    # Windows 平台特定设置
    set(CMAKE_PREFIX_PATH "D:/Qt/6.11.1/mingw_64")  # 替换为你的 Qt 安装路径!!!非常重要 否则没法构建项目
endif()

# 查找 Qt6 组件（通用组件 + 串口）
find_package(Qt6 REQUIRED COMPONENTS Core Widgets SerialPort)

# 添加可执行文件（将 .qrc 文件直接加入，AUTORCC 会处理）
add_executable(${PROJECT_NAME}
    main.cpp
    mainwindow.cpp
    mainwindow.h
    LabeledComboBox.h
    qtsource/resources.qrc   # 资源文件
)

# Windows 专用设置
if(WIN32)
    # 标记为 Windows 应用程序（无控制台窗口）
    set_target_properties(${PROJECT_NAME} PROPERTIES WIN32_EXECUTABLE ON)
    
    # 自动调用 windeployqt 部署 Qt 依赖
    find_program(WINDEPLOYQT_EXECUTABLE windeployqt)
    if(WINDEPLOYQT_EXECUTABLE)
        add_custom_command(TARGET ${PROJECT_NAME} POST_BUILD
            COMMAND "${WINDEPLOYQT_EXECUTABLE}"
                --no-compiler-runtime
                --no-system-d3d-compiler
                "$<TARGET_FILE:${PROJECT_NAME}>"
            COMMENT "正在为 ${PROJECT_NAME} 自动部署 Qt 依赖库..."
        )
    else()
        message(WARNING "找不到 windeployqt.exe，请确保 Qt bin 目录已添加到 PATH")
    endif()
endif()

# Linux / Unix 专用设置（可根据需要补充）
if(UNIX AND NOT APPLE)
    # 例如：设置 RPATH 或安装路径等，这里留空
    # 如果需要支持桌面文件，可以在这里添加
endif()

# 链接 Qt6 库（所有平台通用）
target_link_libraries(${PROJECT_NAME} PRIVATE Qt6::Core Qt6::Widgets Qt6::SerialPort)