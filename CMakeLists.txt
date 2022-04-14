cmake_minimum_required(VERSION 3.18)

project(DS-Lab-Assignment
        VERSION 0.1
        DESCRIPTION "Main Lab Assignment"
        LANGUAGES C CXX)

# set target names

# server app
set(TARGET_SERVER server)

# libraries
set(TARGET_NET_UTILS netUtils)
set(TARGET_DBMS dbms)

if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    set(CMAKE_C_STANDARD 11)
    set(CMAKE_CXX_STANDARD 11)
endif()

# library code
add_subdirectory(src)

# executable code
add_subdirectory(app)

# testing available only if this is the main app and explicitly required
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME AND BUILD_TESTING STREQUAL "ON")
    enable_testing()
#   GoogleTest framework
    add_subdirectory(extern/googletest)
#   unittest code
    add_subdirectory(test)
endif()
