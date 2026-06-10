cmake_minimum_required(VERSION 3.5)

project(BLACKJACKPP LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SFML_DIR "${CMAKE_CURRENT_SOURCE_DIR}/SFML-2.6.2-windows-gcc-13.1.0-mingw-64-bit/SFML-2.6.2/lib/cmake/SFML")

find_package(SFML 2.6 COMPONENTS graphics window system audio REQUIRED)

add_executable(BLACKJACKPP
    src/main.cpp
    src/TextureManager.h
    src/TextureManager.cpp
    src/Card.h
    src/Card.cpp
    src/Hand.h
    src/Hand.cpp
    src/Deck.h
    src/Deck.cpp
    src/Button.h
    src/Button.cpp
)

target_include_directories(BLACKJACKPP PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/src)

target_link_libraries(BLACKJACKPP sfml-graphics sfml-window sfml-system sfml-audio)

file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/textures DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/sounds DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
