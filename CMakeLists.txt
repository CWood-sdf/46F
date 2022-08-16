cmake_minimum_required(VERSION 3.22)
set(CMAKE_COMPILER_IS_GNUCXX ON)

set(CMAKE_CXX_FLAGS "-target thumbv7-none-eabi -fshort-enums -Wno-unknown-attributes -U__INT32_TYPE__ -U__UINT32_TYPE__ -D__INT32_TYPE__=long -D__UINT32_TYPE__='unsigned long'  -march=armv7-a -mfpu=neon -mfloat-abi=softfp -Os -Wall -Werror=return-type -fno-rtti -fno-threadsafe-statics -fno-exceptions  -std=gnu++2a -ffunction-sections -fdata-sections -DVexV5 -Iinclude -I. -Iinclude/lvgl -Iinclude/lvgl/src -I\"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk/vexv5/include\" -I\"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk/vexv5/clang/8.0.0/include\" -I\"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk/vexv5/gcc/include\"  -I\"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk/vexv5/gcc/include/c++/4.9.3\"  -I\"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk/vexv5/gcc/include/c++/4.9.3/arm-none-eabi/armv7-ar/thumb\" -c")

project (46F)

add_executable(46F src/main.cpp "include/AI/BasicAiTask.h")

set(CMAKE_CXX_COMPILER   
	"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\toolchain\\vexv5\\win32\\clang\\bin\\clang.exe")

include_directories(46F "${CMAKE_SOURCE_DIR}\\include"
						"${CMAKE_SOURCE_DIR}\\build-lvgl"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\clang\\8.0.0\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\include"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include\\c++\\4.9.3"
						"C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\include\\c++\\4.9.3\\arm-none-eabi\\armv7-ar\\thumb"
						"C:\\Users\\woodc\\OneDrive\\GitHub\\Libs-Actual\\Libs")


set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED true)

set_property(TARGET 46F 
	PROPERTY CXX_STANDARD 17)

target_link_libraries(46F "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\libv5rt.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libc.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libg.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libgcc.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libm.a"
						  "C:\\Program Files (x86)\\VEX Robotics\\VEXcode Pro V5\\sdk\\vexv5\\gcc\\libs\\libstdc++.a"
						  "${PROJECT_SOURCE_DIR}\\build-lvgl\\liblvgl.a")