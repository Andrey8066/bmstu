cmake_minimum_required(VERSION 3.16)
project(Code VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

# Добавляем исходные файлы для загрузки OSM данных
add_executable(osm_loader
    src/main.cpp
    src/osm_data_loader.cpp
)

# Подключение Qt
find_package(Qt6 REQUIRED COMPONENTS Core Network Sql Widgets)

target_link_libraries(osm_loader PRIVATE Qt::Core Qt::Network Qt::Sql Qt::Widgets)

# Указываем директории исходных файлов
target_include_directories(osm_loader PRIVATE src)


