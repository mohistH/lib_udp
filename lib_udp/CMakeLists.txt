cmake_minimum_required(VERSION 3.14)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET(CMAKE_C_COMPILER /usr/bin/clang)
	SET(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()



# ---------------------------------------------------------------------------------------------------
# 1. set name
project(oct_udp)

# ---------------------------------------------------------------------------------------------------
#4. set some options
option(example_shared 			"an example to show how to use dynamic library" ON)
option(lib_shared 				"to generate dynamic lbrary" 					ON)
# option(lib_mfc 					"to generate a dynamic library to support MFC" 	OFF)


# lib type 
# ---------------------------------------------------------------------------------------------------
if (lib_shared)
	set(lib_udp_type SHARED)
else()
	set(lib_udp_type STATIC)
endif(lib_shared)

# ---------------------------------------------------------------------------------------------------
# 2. to get all source files
# set header files
# -------------------------------------------------------------------------------------
file(GLOB_RECURSE udp_inc ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# set source files
# -------------------------------------------------------------------------------------
file(GLOB_RECURSE udp_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)


# ---------------------------------------------------------------------------------------------
set(publish_path  ${CMAKE_CURRENT_SOURCE_DIR}/publish)

message("")
message(${publish_path})
message("")

# 64位
if (CMAKE_CL_64)
    # 设置 可执行程序输出目录
	set(publish_bin_debug 			${publish_path}/x64/bin/debug)
	set(publish_bin_release 		${publish_path}/x64/bin/release)

    # 设置库文件输出目录
	set(publish_lib_debug 			${publish_path}/x64/lib/debug)
	set(publish_lib_release 		${publish_path}/x64/lib/release)

    set(publih_lib_include          ${publish_path}/x64/include)

	set(PROJECT_NAME 				${PROJECT_NAME}64)
# ---------------------------------------------------------------------------------------------
# 32
else()
 # 设置 可执行程序输出目录
	set(publish_bin_debug 			${publish_path}/x86/bin/debug)
	set(publish_bin_release 		${publish_path}/x86/bin/release)

 # 设置库文件输出目录
	set(publish_lib_debug 			${publish_path}/x86/lib/debug)
	set(publish_lib_release 		${publish_path}/x86/lib/release)

    set(publih_lib_include          ${publish_path}/x86/include)
endif()
# ---------------------------------------------------------------------------------------------

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 				${publish_lib_debug})    
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 				${publish_lib_release})


set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 				${publish_bin_debug})   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 				${publish_bin_release}) 


set(CMAKE_DEBUG_POSTFIX "D") 


# build a library for udp
function(lib_udp_on_win)
	# dynamic library
	# ---------------------------------------------------------------------------------------------------

	# compiler is vs
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 		
		# build program
		add_library( ${PROJECT_NAME} ${lib_udp_type} ${udp_src} ${udp_inc})

		# .h and .cxx files
		target_sources(${PROJECT_NAME} PRIVATE ${udp_src} ${udp_inc})

		# set 
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/udp)

		file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/iudp.h DESTINATION ${publih_lib_include})

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

		# link library
		target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)


	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endfunction(lib_udp_on_win)


# ---------------------------------------------------------------------------------------------------
# build a library on linux
function(lib_udp_on_linux)
	
	# compiler is clang
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
			
		set(${PROJECT_NAME} "udp")

		# 6.2 to create an tatic library 
		add_library( ${PROJECT_NAME} ${lib_udp_type} )

		# link library
		target_link_libraries(${PROJECT_NAME} PRIVATE pthread)

		# .h and .cxx files
		target_sources(${PROJECT_NAME} PRIVATE ${udp_src} ${udp_inc})

		# set 
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/udp)

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")



endfunction (lib_udp_on_linux)







# ---------------------------------------------------------------------------------------------------
function(lib_udp_example_shared)
		#7.1 set src path
	set (exam_src 
		${CMAKE_CURRENT_SOURCE_DIR}/example/main.cc
	)

	set(exam_name "call_lib")

	# 7.3 to get an executable program
	add_executable(${exam_name} ${exam_src} ${udp_inc})
	target_include_directories(${exam_name} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)
	target_link_libraries(${exam_name} PRIVATE ${PROJECT_NAME})

endfunction(lib_udp_example_shared)


# build library 
# ---------------------------------------------------------------------------------------------
if (lib_shared)
	if (CMAKE_SYSTEM_NAME MATCHES "Linux")
		lib_udp_on_linux()
	elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
		lib_udp_on_win()
	endif (CMAKE_SYSTEM_NAME MATCHES "Linux")

	lib_udp_example_shared()
endif(lib_shared)















#------------------------------------------------------------------------------------

# if(WIN32)
# 	# 
# 	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug)
# 	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/Release)

# 	# 
# 	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/buiold/lib/Debug)
# 	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/buiold/lib/Release)
# endif(WIN32)



# 判断操作系统

IF (CMAKE_SYSTEM_NAME MATCHES "Linux")
	
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "Windows")
	
ELSEIF (CMAKE_SYSTEM_NAME MATCHES "FreeBSD")
	
ELSE ()
	MESSAGE(STATUS "other platform: ${CMAKE_SYSTEM_NAME}")
ENDIF (CMAKE_SYSTEM_NAME MATCHES "Linux")


## 判断编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
# using Clang
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
# using GCC
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
# using Intel C++
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
# using Visual Studio C++
endif()


