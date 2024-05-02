#!/bin/bash
g++ -c snowflake.cpp
g++ snowflake.o -o run -lsfml-graphics -lsfml-window -lsfml-system