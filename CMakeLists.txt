cmake_minimum_required(VERSION 3.24)
project(HW5___02360360)

set(CMAKE_CXX_STANDARD 14)

include_directories(.)

add_executable(HW5___02360360
        codeGvisitor.cpp
        codeGvisitor.hpp
        lex.yy.c
        main.cpp
        nodes.cpp
        nodes.hpp
        output.cpp
        output.hpp
        parser.tab.c
        parser.tab.h
        SemanticVisitor.cpp
        SemanticVisitor.hpp
        SymbolTable.cpp
        SymbolTable.hpp
        visitor.hpp)
