cmake_minimum_required(VERSION 3.10.2)
project(VirtApp)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /EHsc")
set(Boost_USE_MULTITHREADED ON)
 
find_package(Threads REQUIRED)
find_package(OpenSSL REQUIRED)
find_package(Tesseract CONFIG REQUIRED)
find_package(Boost COMPONENTS system REQUIRED)
find_package(CURL)
find_package(mailio CONFIG REQUIRED)
find_package(TgBot CONFIG REQUIRED)

# Interception library - установлена в C:/libs/interception
set(INTERCEPTION_INCLUDE_DIR "C:/libs/interception/Interception/library" CACHE PATH "Path to Interception include directory")
set(INTERCEPTION_LIB "C:/libs/interception/Interception/library/x64/interception.lib" CACHE FILEPATH "Path to Interception library")

# SQLite Modern C++ header
set(SQLITE_MODERN_CPP_DIR "C:/libs/sqlite_modern_cpp" CACHE PATH "Path to sqlite_modern_cpp header")

include_directories(
    ${CMAKE_SOURCE_DIR}/src
    ${INTERCEPTION_INCLUDE_DIR}
    ${SQLITE_MODERN_CPP_DIR}
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
    TgBot::TgBot
    mailio::mailio 
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
