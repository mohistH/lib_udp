cmake_minimum_required(VERSION 3.1)

if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")
	SET(CMAKE_C_COMPILER /usr/bin/clang)
	SET(CMAKE_CXX_COMPILER /usr/bin/clang++)
endif()



# ---------------------------------------------------------------------------------------------------
# 1. set name
project(lib_udp)


# set var environment
set(CMAKE_MODULE_PATH "${CMAKE_CURRENT_SOURCE_DIR}/cmake;${CMAKE_MODULE_PATH}")
include(spdlog)



# ---------------------------------------------------------------------------------------------------
# 2. to get all source files
# set header files
# -------------------------------------------------------------------------------------
file(GLOB_RECURSE udp_inc ${CMAKE_CURRENT_SOURCE_DIR}/include/*.h)

# set source files
# -------------------------------------------------------------------------------------
file(GLOB_RECURSE udp_src ${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp)


# ---------------------------------------------------------------------------------------------------
# 3. to attach to path of includes direcotries
include_directories(
"${CMAKE_CURRENT_SOURCE_DIR}/include"
"${CMAKE_CURRENT_SOURCE_DIR}/ext/spdlog-1.x/include"
)


# ---------------------------------------------------------------------------------------------------
#4. set some options
option(example_shared 			"an example to show how to use dynamic library" OFF)
option(lib_shared 				"to generate dynamic lbrary" 					OFF)
option(lib_mfc 					"to generate a dynamic library to support MFC" 	ON)


# lib type 
# ---------------------------------------------------------------------------------------------------
if (lib_shared)
	set(lib_udp_type SHARED)
else()
	set(lib_udp_type STATIC)
endif(lib_shared)




# build a library for udp
function(lib_udp_on_win)
	# dynamic library
	# ---------------------------------------------------------------------------------------------------

	# compiler is vs
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
 		
		# build program
		add_library( ${PROJECT_NAME} ${lib_udp_type} ${udp_src} )

		# .h and .cxx files
		target_sources(${PROJECT_NAME} PRIVATE ${udp_src} ${udp_inc})

		# set 
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

		# link library
		target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)

		# to quto third project
		# ---------------------------------------------------------------------------------------------------
		# spdlog
		include(spdlog)
		target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endfunction(lib_udp_on_win)




# build a mfc library
function(lib_udp_on_win_mfc)
	# build a library to support MFC
	# ---------------------------------------------------------------------------------------------------
	if ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
		set(CMAKE_MFC_FLAG 2)
		set(CMAKE_CXX_FLAGS_RELEASE "/MT")
     	set(CMAKE_CXX_FLAGS_DEBUG "/MTd")
		
		# set charators unicode
		Add_Definitions(-DUNICODE -D_UNICODE)
		Add_Definitions(-D_AFC_DLL)
		SET(CMAKE_MFC_FLAG 2)	

			
		# 6.2 to create an tatic library 
		add_library( ${PROJECT_NAME} ${lib_udp_type} ${udp_src} )
		

		# link library
		target_link_libraries(${PROJECT_NAME} PRIVATE ws2_32)

		# .h and .cxx files
		target_sources(${PROJECT_NAME} PRIVATE ${udp_src} ${udp_inc})

		# set 
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)
			
		# 6.4 set others 
		set_property(TARGET ${PROJECT_NAME} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/${CMAKE_CFG_INTDIR})

		# to quto third project
		# ---------------------------------------------------------------------------------------------------
		# spdlog
		include(spdlog)
		target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
endfunction(lib_udp_on_win_mfc)



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
		target_include_directories(${PROJECT_NAME} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/include/)

		# use c++11
		target_compile_features(${PROJECT_NAME} PRIVATE cxx_std_11)

		# spdlog
		include(spdlog)
		target_link_libraries(${PROJECT_NAME} PRIVATE spdlog::spdlog)

	endif("${CMAKE_CXX_COMPILER_ID}" STREQUAL "Clang")



endfunction (lib_udp_on_linux)







# ---------------------------------------------------------------------------------------------------
function(lib_udp_example_shared)
		#7.1 set src path
	set (exam_src 
		${CMAKE_CURRENT_SOURCE_DIR}/example/main.cpp
	)


	# 7.3 to get an executable program
	add_executable(exam_lib_dyn ${exam_src})

	#7.4 to link library
	IF (CMAKE_SYSTEM_NAME MATCHES "Windows")
		if (lib_mfc)
			# todo
		else()
			target_link_libraries(exam_lib_dyn ${PROJECT_NAME})
		endif()
	ELSE()
		target_link_libraries(exam_lib_dyn lib_udp)
	ENDIF (CMAKE_SYSTEM_NAME MATCHES "Windows")

endfunction(lib_udp_example_shared)



## ---------------------------------------------------------------------------------------------
## end function definition
## ---------------------------------------------------------------------------------------------



# build library 
# ---------------------------------------------------------------------------------------------
if (lib_shared)
	if (CMAKE_SYSTEM_NAME MATCHES "Linux")
		lib_udp_on_linux()
	elseif (CMAKE_SYSTEM_NAME MATCHES "Windows")
		lib_udp_on_win()
	endif (CMAKE_SYSTEM_NAME MATCHES "Linux")
endif(lib_shared)

## mfc
if (lib_mfc)
	if (CMAKE_SYSTEM_NAME MATCHES "Windows")
		lib_udp_on_win_mfc()
	endif (CMAKE_SYSTEM_NAME MATCHES "Windows")
endif(lib_mfc)



# ---------------------------------------------------------------------------------------------
#7.  an example , to show how to use dynamic libary
if (example_shared)
	lib_udp_example_shared()
endif(example_shared)














#------------------------------------------------------------------------------------

if(WIN32)
	# 
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/build/Debug)
	set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/build/Release)

	# 
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_DEBUG ${CMAKE_CURRENT_SOURCE_DIR}/buiold/lib/Debug)
	set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY_RELEASE ${CMAKE_CURRENT_SOURCE_DIR}/buiold/lib/Release)
endif(WIN32)



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


