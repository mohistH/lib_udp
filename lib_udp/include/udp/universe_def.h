#ifndef _universe_def_h
#define _universe_def_h


/// c/c++ run time library
#ifdef _UNICODE
	#ifndef UNICODE
		#define UNICODE
	#endif
#endif

/// windows 
#ifdef UNICODE
	#ifndef _UNICODE
		#define _UNICODE
	#endif
#endif



/// cpp_version to check if it suppports c++11
///---------------------------------------------------------------------------------------------
#if __cplusplus >= 201103L
	#define has_cxx_11 
#endif /// ! __cplusplus >= 201103L


/// compiler
///---------------------------------------------------------------------------------------------
#if defined(__clang__) || defined(__GNUC__)
#define compiler_is_clang
/// clang or gcc(++)
#elif defined(_MSC_VER)		/// use vs compiler

	#ifndef compiler_is_vs
		#define compiler_is_vs
	#endif ///! compiler_is_vs
///--------------------------------------------

	#if 1900 <= _MSC_VER	/// 1900 = vs2015
		#ifndef has_cxx_11
			#define has_cxx_11
		#endif /// 
	#endif


#endif ///! defined(__clang__) || defined(__GNUC__)


/// os
///---------------------------------------------------------------------------------------------
#if defined(_WIN32) || defined(_WIN64)

#ifndef os_is_win
	#define os_is_win 
#else
#endif /// os_is_win 

#elif defined(_linux) || defined(_linux_) || defined(__linux) || defined (_unix_)

	#ifndef os_is_linux
		#define os_is_linux
	#else
	
	#endif //// os_is_linux 

#elif defined(__APPLE__)

	#ifndef os_is_osx
		#define os_is_osx
	#else
		/// # 
	#endif //// os_is_osx 

#endif /// !defined(_linux) || defined(_linux_) || defined(__linux) || defined (_unix_)


///#ifdef os_is_win
///	#ifndef _mfc_is_on_
///		/// #define used_in_mfc
///	#endif /// _mfc_is_on_
///#endif /// os_is_win



#endif /// _universe_def_h
