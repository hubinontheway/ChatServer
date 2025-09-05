cmake_minimum_required(VERSION 3.15)
project(ChatServer)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/include)

find_library(ODB_LIB odb REQUIRED)
find_package(CURL REQUIRED)
find_library(ODB_MYSQL_LIB odb-mysql REQUIRED)
find_library(LLHTTP_LIB llhttp PATHS ${CMAKE_SOURCE_DIR}/lib NO_DEFAULT_PATH)

add_executable(ChatServer src/net/socket.cpp
        src/net/epoll.cpp
        src/net/channel.cpp
        src/net/event_loop.cpp
        src/main.cpp
        src/server/chat_server.cpp
        src/http/http_request_parser.cpp
        src/http/http_response.cpp
        src/http/llhttp.c
        src/http/router.cpp
        src/web/chat_handlers.cpp
        src/web/user_handlers.cpp
        src/service/chat_service.cpp
        src/net/connection.cpp
        src/repository/user-odb.cxx)

# 链接库
target_link_libraries(ChatServer
        ${ODB_LIB}
        ${ODB_MYSQL_LIB}
        ${LLHTTP_LIB}
        ${CURL_LIBRARIES}
)
