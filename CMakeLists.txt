cmake_minimum_required(VERSION 3.1...3.24)

IF (!WIN32)
	set(CMAKE_CXX_COMPILER "clang++")
	set(CMAKE_C_FLAGS_DEBUG "-g -DDEBUG")
	set(CMAKE_CXX_FLAGS_DEBUG "-g -DDEBUG")
ENDIF()
set(CMAKE_CXX_STANDARD 20)

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

IF (WIN32)
	target_include_directories(Tiler PUBLIC C:/Library/vcpkg/installed/x64-windows/include/)
	target_link_directories(Tiler PUBLIC C:/Library/vcpkg/installed/x64-windows/lib/)
	
	target_link_libraries(Tiler libcurl)
	target_link_libraries(Tiler libpng16)
	target_link_libraries(Tiler tiff)
ELSE()
	include_directories(/usr/local/include)
	link_directories(/usr/local/lib)

	target_link_libraries(Tiler curl)
	target_link_libraries(Tiler libpng.a)
	target_link_libraries(Tiler libtiff.a)
	target_link_libraries(Tiler libfmt.a)
ENDIF()