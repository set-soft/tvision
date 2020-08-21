cmake_minimum_required(VERSION 3.0)

INCLUDE(CheckIncludeFiles)
INCLUDE(CheckFunctionExists)
INCLUDE(TestBigEndian)

set(TV_DEP_LIBS)

# TODO: What's is SSC?
set(HAVE_SSC 0)

find_package(Curses)
if(CURSES_HAVE_NCURSES_H)
	set(HAVE_NCURSES 1)
	include_directories(${CURSES_INCLUDE_DIRS})
	list(APPEND TV_DEP_LIBS ${CURSES_LIBRARIES})
endif(CURSES_HAVE_NCURSES_H)
CHECK_FUNCTION_EXISTS(define_key "ncurses.h" HAVE_DEFINE_KEY)

set(HAVE_X11 0)
find_package(X11)
if(X11_FOUND)
	CHECK_INCLUDE_FILES("${X11_INCLUDE_DIR}/Xmu/Atoms.h" HAVE_X11)
	if(HAVE_X11)
		include_directories(${X11_INCLUDE_DIR})
		list(APPEND TV_DEP_LIBS ${X11_LIBRARIES})
	endif(HAVE_X11)
endif(X11_FOUND)

CHECK_FUNCTION_EXISTS(outb "sys/io.h" HAVE_OUTB_IN_SYS)

TEST_BIG_ENDIAN(TV_BIG_ENDIAN)

find_package(Threads)
if(CMAKE_USE_PTHREADS_INIT)
	set(HAVE_LINUX_PTHREAD 1)
	list(APPEND TV_DEP_LIBS ${CMAKE_THREAD_LIBS_INIT})
endif(CMAKE_USE_PTHREADS_INIT)

# TODO: Doesn't work with cross-compilation
if("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")
	set(HAVE_64BITS_POINTERS 1)
endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "8")

# TVOS_
if(UNIX)
	set(TVOS_UNIX 1)
endif(UNIX)
if(WIN32)
	set(TVOS_Win32 1)
endif(WIN32)
# TVOSf_
if("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
	set(TVOSf_Linux 1)
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Linux")
if("${CMAKE_SYSTEM_NAME}" MATCHES "FreeBSD")
	set(TVOSf_FreeBSD 1)
endif("${CMAKE_SYSTEM_NAME}" MATCHES "FreeBSD")
if("${CMAKE_SYSTEM_NAME}" MATCHES "NetBSD")
	set(TVOSf_NetBSD 1)
endif("${CMAKE_SYSTEM_NAME}" MATCHES "NetBSD")
if("${CMAKE_SYSTEM_NAME}" MATCHES "Solaris")
	set(TVOSf_Solaris 1)
endif("${CMAKE_SYSTEM_NAME}" MATCHES "Solaris")
if(APPLE)
	set(TVOSf_Darwin 1)
endif(APPLE)
# TVCPU_x86 is the only one in use
if("${CMAKE_SYSTEM_PROCESSOR}" EQUAL "i386")
	if("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
		set(TVCPU_x86 1)
	endif("${CMAKE_SIZEOF_VOID_P}" EQUAL "4")
endif("${CMAKE_SYSTEM_PROCESSOR}" EQUAL "i386")
# TVComp_
if(CMAKE_COMPILER_IS_GNUCC)
	set(TVComp_GCC 1)
endif(CMAKE_COMPILER_IS_GNUCC)
if("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
	# Clang looks like GCC
	set(TVComp_GCC 1)
endif("${CMAKE_C_COMPILER_ID}" MATCHES "Clang")
if("${CMAKE_C_COMPILER_ID}" EQUAL "MSVC")
	set(TVComp_MSC 1)
endif("${CMAKE_C_COMPILER_ID}" EQUAL "MSVC")
if(BORLAND)
	set(TVComp_BCPP 1)
endif(BORLAND)
#TVCompf_
if(MINGW)
	set(TVCompf_MinGW 1)
endif(MINGW)
if(CYGWIN)
	set(TVCompf_Cygwin 1)
endif(CYGWIN)
if(NOT ENABLE_WINGR_DRIVER)
	set(TV_Disable_WinGr_Driver 1)
endif(NOT ENABLE_WINGR_DRIVER)
configure_file ("${CMAKE_SOURCE_DIR}/include/tv/configtv.h.in"
                "${CMAKE_BINARY_DIR}/include/tv/configtv.h" )


