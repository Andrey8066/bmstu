cmake_minimum_required(VERSION 3.16)
project(Code VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Добавляем исходные файлы для загрузки OSM данных
add_executable(osm_loader
    src/main.cpp
    src/osm_data_loader.cpp
    src/dem_reader.cpp
    src/mainwindow.hpp
    src/mainwindow.cpp
    src/glwidget.hpp
    src/glwidget.cpp
)

# Подключение Qt
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Widgets OpenGLWidgets)
find_package(GDAL REQUIRED)

target_link_libraries(osm_loader PRIVATE GDAL::GDAL Qt::Core Qt::Network Qt::Sql Qt::Widgets Qt::OpenGLWidgets)

# Указываем директории исходных файлов
target_include_directories(osm_loader PRIVATE src)


