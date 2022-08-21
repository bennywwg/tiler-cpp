cmake_minimum_required(VERSION 3.1...3.24)

set(CMAKE_CXX_COMPILER "clang++")
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")

include_directories(/usr/local/include)
link_directories(/usr/local/lib)

project(
  Tiler
  VERSION 1.0
  LANGUAGES CXX
)

add_executable(Tiler
    main.cpp
    FilesystemTileSource.cpp
    Image.cpp
    HTTPTileReader.cpp
    ReadURI.cpp
)

target_include_directories(Tiler PUBLIC /opt/homebrew/Cellar/glm/0.9.9.8/include/)
target_include_directories(Tiler PUBLIC /opt/homebrew/Cellar/curl/7.84.0/include/)
target_include_directories(Tiler PUBLIC /opt/homebrew/Cellar/libpng/1.6.37/include/)
target_include_directories(Tiler PUBLIC /opt/homebrew/Cellar/libtiff/4.4.0_1/include/)
target_include_directories(Tiler PUBLIC /opt/homebrew/Cellar/fmt/8.1.1_1/include/)

target_link_directories(Tiler PUBLIC /opt/homebrew/Cellar/libpng/1.6.37/lib/)
target_link_directories(Tiler PUBLIC /opt/homebrew/Cellar/libtiff/4.4.0_1/lib/)
target_link_directories(Tiler PUBLIC /opt/homebrew/Cellar/fmt/8.1.1_1/lib/)

target_link_libraries(Tiler curl)
target_link_libraries(Tiler libpng.a)
target_link_libraries(Tiler libtiff.a)
target_link_libraries(Tiler libfmt.a)