cmake_minimum_required(VERSION 3.10) 

SET(CMAKE_TOOLCHAIN_FILE "build/ToolChain.cmake")
#SET(CMAKE_SYSTEM_NAME "Linux")
#SET(CMAKE_C_COMPILER "gcc")
#SET(CMAKE_CXX_COMPILER "g++")

project (net)
 
#set(CMAKE_INSTALL_PREFIX ${CMAKE_INSTALL_PREFIX}/net) #CMAKE_INSTALL_PREFIX 外面设置，内部设置无效

# 设置C++标准为C++11
set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
message("PROJECT_SOURCE_DIR is: ${PROJECT_SOURCE_DIR} ${CMAKE_INSTALL_PREFIX}")


# 根据平台添加特定源文件  
if (WIN32)  
    #list(APPEND src_list  TcpSocketWin.cpp  # 仅在 Windows 上的源文件  )
    # 收集所有源文件  
    file(GLOB src_list 
        TcpSocketWin.cpp  # 仅在 Windows 上的源文件
        )  # 可以根据你的目录结构调整 
else()  
    # 收集所有源文件  
    file(GLOB src_list 
        UdpSocket.cpp
        TcpSocket.cpp  # 在非 Windows 上的源文件  
        )  # 可以根据你的目录结构调整 
    #list(APPEND src_list  TcpSocket.cpp  # 在非 Windows 上的源文件  )  
endif()
#set(src_list
#    src/xxx.cpp
#)

# 定义库  
add_library(net ${src_list}) 

target_include_directories(net PUBLIC
    ${PROJECT_SOURCE_DIR}
    ${PROJECT_SOURCE_DIR}/include
)


# 设置库输出目录  
#SET(LIBRARY_OUTPUT_PATH CMAKE_LIBRARY_OUTPUT_DIRECTORY ${PROJECT_SOURCE_DIR}/build/lib)
set(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)


# 安装库文件  
install(TARGETS net  
        DESTINATION net/lib)  # 库文件的安装目录  

# 安装头文件  
file(GLOB PUBLIC_HEADERS "${PROJECT_SOURCE_DIR}/include/*.h")
install(FILES ${PUBLIC_HEADERS}  
        DESTINATION net/include)  # 头文件的安装目录



#SET_TARGET_PROPERTIES(base64_static PROPERTIES OUTPUT_NAME base64)
#SET(LIBRARY_OUTPUT_PATH ${PROJECT_SOURCE_DIR}/build/lib)

#TARGET_LINK_LIBRARIES(webrtc pthread -lm)

#TARGET_LINK_LIBRARIES(crnode log4cpp thrift)










