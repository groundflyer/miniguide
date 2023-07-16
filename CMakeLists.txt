cmake_minimum_required(VERSION 3.16)

project(iiguide CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(SOURCE_FILES
  src/main.cpp
  src/layout.cpp
  src/parser.cpp
  src/Section.cpp
)

find_package(Qt5 COMPONENTS Xml Widgets REQUIRED)

include_directories(${Qt5Widgets_INCLUDE_DIRS} ${Qt5Xml_INCLUDE_DIRS})
add_definitions(${Qt5Widgets_DEFINITIONS} ${Qt5Xml_DEFINISTION})

set(CMAKE_AUTOMOC ON) # For meta object compiler

add_executable(${PROJECT_NAME}
    ${SOURCE_FILES} ${META_FILES_TO_INCLUDE}
)

target_link_libraries(${PROJECT_NAME} Qt5::Widgets Qt5::Xml)
