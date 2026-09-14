cmake_minimum_required(VERSION 3.16)
project(OSMDataLoader LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)

add_executable(osm_loader
    main.cpp
    osm_data_loader.cpp
)

find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Widgets)

target_link_libraries(osm_loader PRIVATE Qt::Core Qt::Network Qt::Sql Qt::Widgets)
