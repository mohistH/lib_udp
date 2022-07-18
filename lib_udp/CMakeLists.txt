cmake_minimum_required(VERSION 3.14)
# ---------------------------------------------------------------------------------------------------
# 1. set name
project(lib_commu)

# ---------------------------------------------------------------------------------------------------
#4. set some options
option(example_shared 			"an example to show how to use dynamic library" ON)
option(lib_shared 				"ON=SHARED, OFF=STATIC" 					ON)



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
file(GLOB_RECURSE lib_inc_files ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# set source files
# -------------------------------------------------------------------------------------
file(GLOB_RECURSE lib_src_files ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cc)


set(CMAKE_MODULE_PATH 
${CMAKE_MODULE_PATH}
${CMAKE_CURRENT_SOURCE_DIR}/cmake
)
FIND_PACKAGE(libuv64 REQUIRED)



set(compiler_name "")

## 判断编译器
if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	set(compiler_name "clang")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
	set(compiler_name "gnu")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Intel")
	set(compiler_name "inter")
elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
	if(MSVC_VERSION EQUAL 1400)
		set(compiler_name "vc8")
	elseif(MSVC_VERSION EQUAL 1500)
		set(compiler_name "vc9")
	elseif(MSVC_VERSION EQUAL 1600)
		set(compiler_name "vc10")
	elseif(MSVC_VERSION EQUAL 1700)
		set(compiler_name "vc11")
	elseif(MSVC_VERSION EQUAL 1800)
		set(compiler_name "vc12")
	elseif(MSVC_VERSION EQUAL 1900)
		set(compiler_name "vc14")
	endif()

endif()

# ---------------------------------------------------------------------------------------------
set(publish_path  										${CMAKE_CURRENT_SOURCE_DIR}/publish/${compiler_name})


# 64位 还是32
set(platform_info 				"")

# 64位
if (CMAKE_CL_64)
	set(platform_info 				"x64")
# ---------------------------------------------------------------------------------------------
# 32
else()
	set(platform_info 				"x86")
endif()
# ---------------------------------------------------------------------------------------------

set(publish_path ${publish_path}/${platform_info})


 # 设置 可执行程序输出目录
 set(publish_bin_debug 			${publish_path}/bin/debug)
 set(publish_bin_release 		${publish_path}/bin/release)

# 设置库文件输出目录
 set(publish_lib_debug 			${publish_path}/lib/debug)
 set(publish_lib_release 		${publish_path}/lib/release)

 set(publih_lib_include          ${publish_path}/include)








# ---------------------------------------------------------------------------------------------
# 设置项目名
set(PROJECT_NAME 				${PROJECT_NAME})

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG 				${publish_lib_debug})    
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE 				${publish_lib_release})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG 				${publish_bin_debug})   
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE 				${publish_bin_release}) 

set(CMAKE_DEBUG_POSTFIX "D") 
set(CMAKE_MODULE_PATH ${CMAKE_CURRENT_SOURCE_DIR}/cmake)

FIND_PACKAGE(libuv64 REQUIRED)



# build a library for udp
function(lib_udp_on_win)
	# build program
	add_library( ${PROJECT_NAME} ${lib_udp_type} ${lib_inc_files} ${lib_src_files})

	add_definitions(-DWIN32_LEAN_AND_MEAN)

	# set 
	target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

	file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/include/icommonunication.h DESTINATION ${publih_lib_include})

	if (lib_uv_has_found)
		target_include_directories(${PROJECT_NAME}
		# PRIVATE ${uv_inc_files}
		PRIVATE ${uv_inc_path}
		)
		target_link_libraries(${PROJECT_NAME} PRIVATE ${uv_lib_files})
		file(COPY ${uv_bin_debug_file} DESTINATION ${publish_bin_debug})
		file(COPY ${uv_bin_release_file} DESTINATION ${publish_bin_release})
	endif()

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
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/ext/lib_uv/include)

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")



endfunction (lib_udp_on_linux)




if (uv_exam)

	set(uv_example
	${CMAKE_CURRENT_SOURCE_DIR}/uv_example/exam.cc
	)

	# build program
	add_executable( uv_demo ${uv_example})

	add_definitions(-DWIN32_LEAN_AND_MEAN)

	# .h and .cxx files
	# target_sources(${PROJECT_NAME} PRIVATE ${lib_inc_files} ${lib_src_files})

	# set 
	target_include_directories(uv_demo PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include)

	# link library
	# target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)

	if (lib_uv_has_found)
		target_include_directories(uv_demo PRIVATE ${uv_inc_files})
		target_link_libraries(uv_demo PRIVATE ${uv_lib_files})
	endif()
endif(uv_exam)






# ---------------------------------------------------------------------------------------------------
function(lib_udp_example_shared)
		#7.1 set src path
	set (exam_src 
		${CMAKE_CURRENT_SOURCE_DIR}/example/main.cc
		${CMAKE_CURRENT_SOURCE_DIR}/include/icommonunication.h
	)


	set(exam_name "call_lib")

	# 7.3 to get an executable program
	add_executable(${exam_name} ${exam_src} ${uv_inc_files} )


	target_include_directories(${exam_name} 
	PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include
	)

	target_link_libraries(${exam_name} PRIVATE ${PROJECT_NAME})

	if (lib_uv_has_found)
		target_include_directories(${exam_name} PRIVATE ${uv_inc_files})
		target_link_libraries(${exam_name} PRIVATE ${PROJECT_NAME})
		target_link_libraries(${exam_name} PRIVATE ${uv_lib_files})
		endif()

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





