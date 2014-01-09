# Mandatory to supress the huge warning
cmake_minimum_required(VERSION 2.6)

# Location of the lwip source checkout
set(lwip_root ../lwip_ucos2)

# Name of the project
project(cengine)

file(GLOB_RECURSE
	core_files
    ${lwip_root}/src/core/*
    ${lwip_root}/src/api/*
    ${lwip_root}/src/netif/*
)	

file(GLOB_RECURSE
	port_files
    ${lwip_root}/src/contrib/ports/unix/perf.c
    ${lwip_root}/src/contrib/ports/unix/lwip_chksum.c
    ${lwip_root}/src/contrib/ports/unix/sys_arch.c
    ${lwip_root}/src/contrib/ports/unix/netif/*
)	

file(GLOB
    project_files
    main.c
    ping.c)

include_directories(./)
include_directories(${lwip_root}/src/include)
include_directories(${lwip_root}/src/include/ipv4)
include_directories(${lwip_root}/src/include/ipv6)
include_directories(${lwip_root}/src/contrib/ports/unix/include)

find_package (Threads)

# Executable source file definitions
add_executable(
	engine
	${core_files}
    ${port_files}
    ${project_files}
)

target_link_libraries (engine ${CMAKE_THREAD_LIBS_INIT})
