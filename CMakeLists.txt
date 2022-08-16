cmake_minimum_required(VERSION 3.22)
project (46F)

add_executable(46F src/main.cpp)

#set(CMAKE_CXX_COMPILER   
#	"C:\\Program Files\\Microsoft Visual Studio\\2022\\Community\\VC\\Tools\\MSVC\\14.32.31326\\bin\\Hostx86\\x86\\bscmake.exe")

include_directories(46F ${CMAKE_SOURCE_DIR}\\include
						"${CMAKE_SOURCE_DIR}\\build-lvgl"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\clang\\8.0.0\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include\\c++\\4.9.3"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include\\c++\\4.9.3\\arm-none-eabi\\armv7-ar\\thumb"
						"C:\\Users\\woodc\\OneDrive\\GitHub\\Libs-Actual\\Libs")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set_property(TARGET 46F PROPERTY CXX_STANDARD 17)

target_link_libraries(46F "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\libv5rt.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libc.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libg.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libgcc.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libm.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libstdc++.a"
						  "${PROJECT_SOURCE_DIR}\\build-lvgl\\liblvgl.a")