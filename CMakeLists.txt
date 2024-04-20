# 20.04.2024 10:50:25 Genrated by cbp2cmake V2.4.0 Finale X86 
project(crc32 LANGUAGES C)
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
set (CMAKE_INSTALL_PREFIX /usr )
add_executable(crc32 ctools/c_crc32.c ctools/c_md5.c tools.c crc32.c)
install(TARGETS crc32 RUNTIME DESTINATION bin)
