cmake_minimum_required(VERSION 3.10.2)
project(VirtApp)

set(CMAKE_CXX_STANDARD 14)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
set(Boost_USE_MULTITHREADED ON)
 
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Tesseract CONFIG REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package(mailio CONFIG REQUIRED)

# TODO: Update this path to point to your Interception library installation
set(INTERCEPTION_INCLUDE_DIR "C:/path/to/Interception/library" CACHE PATH "Path to Interception include directory")
set(INTERCEPTION_LIB "C:/path/to/Interception/library/x64/interception.lib" CACHE FILEPATH "Path to Interception library")

# TODO: Update this path to point to your TgBot library installation
set(TGBOT_LIB "C:/src/vcpkg/packages/tgbot-cpp_x64-windows/lib/TgBot.lib" CACHE FILEPATH "Path to TgBot library")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${INTERCEPTION_INCLUDE_DIR}
    ${OPENSSL_INCLUDE_DIR}
    ${Boost_INCLUDE_DIRS}
)

if (CURL_FOUND)
    include_directories(${CURL_INCLUDE_DIRS})
    add_definitions(-DHAVE_CURL)
endif()

# Source files
add_executable(VirtApp 
    src/main.cpp 
    src/VirtApp.h 
    src/ScreenScaner.cpp 
    src/ScreenScaner.h 
    src/utils.c 
    src/utils.h
)

target_link_libraries(VirtApp 
    ${INTERCEPTION_LIB}
    ${TGBOT_LIB}
    mailio 
    ${CMAKE_THREAD_LIBS_INIT} 
    ${OPENSSL_LIBRARIES} 
    ${Boost_LIBRARIES} 
    ${CURL_LIBRARIES} 
    Tesseract::libtesseract
)

# Copy runtime dependencies to build directory
add_custom_command(TARGET VirtApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/scripts
        $<TARGET_FILE_DIR:VirtApp>/scripts
    COMMENT "Copying scripts to build directory"
)

add_custom_command(TARGET VirtApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/data
        $<TARGET_FILE_DIR:VirtApp>/data
    COMMENT "Copying data to build directory"
)

add_custom_command(TARGET VirtApp POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy_directory
        ${CMAKE_SOURCE_DIR}/config
        $<TARGET_FILE_DIR:VirtApp>/config
    COMMENT "Copying config to build directory"
)
