cmake_minimum_required(VERSION 3.0.0)
project(GraficoComAvalicaoDeExpressao VERSION 0.1.0)

include(CTest)
enable_testing()

add_executable(GraficoComAvalicaoDeExpressao main.cpp)

find_package(SDL2 CONFIG REQUIRED)
target_link_libraries(GraficoComAvalicaoDeExpressao PRIVATE SDL2::SDL2 SDL2::SDL2main)

add_library(Tinyexpr tinyexpr.c)
target_link_libraries(GraficoComAvalicaoDeExpressao PUBLIC Tinyexpr)


set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)
