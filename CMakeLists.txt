cmake_minimum_required(VERSION 3.5.1)
project(561Assg3)

FIND_PACKAGE(BISON REQUIRED)
SET(BisonOutput ${CMAKE_SOURCE_DIR}/y.tab.cpp)
IF(BISON_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${BisonOutput}
            COMMAND ${BISON_EXECUTABLE}
            -d -h${CMAKE_SOURCE_DIR}/y.tab.h
            -o ${BisonOutput}
            ${CMAKE_SOURCE_DIR}/main.y
            COMMENT "Generating parser"
    )
ENDIF()

FIND_PACKAGE(FLEX REQUIRED)
SET(FlexOutput ${CMAKE_SOURCE_DIR}/lex.yy.cpp)
IF(FLEX_FOUND)
    ADD_CUSTOM_COMMAND(
            OUTPUT ${FlexOutput}
            COMMAND ${FLEX_EXECUTABLE}
            -o${FlexOutput}
            ${CMAKE_SOURCE_DIR}/main.l
            COMMENT "Generating scanner"
    )
ENDIF()


set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
set(SOURCE_FILES
        main.cpp
        lex.yy.cpp
        y.tab.cpp
        ast.h global.h ast.cpp)

add_executable(561Assg3 ${SOURCE_FILES})