
cmake_minimum_required(VERSION 3.14.0)
project(RTOS_simulation)

if(MINGW OR CYGWIN OR WIN32)
	set(UTIL_SEARCH_CMD where)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -ggdb -g -UUSE_STDIO -std=c99")
elseif(UNIX OR APPLE)
	set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -ggdb -g -UUSE_STDIO ")
	set(UTIL_SEARCH_CMD which)
endif()


execute_process(
	COMMAND ${UTIL_SEARCH_CMD} gcc
	OUTPUT_VARIABLE COMPILER
        OUTPUT_STRIP_TRAILING_WHITESPACE
)

set(CMAKE_C_COMPILER ${COMPILER})


option(BUILD_DSP_LIB OFF)

#build dsp library
if(BUILD_DSP_LIB)
set(ROOT ${CMAKE_SOURCE_DIR}/CMSIS_5)
message( STATUS ${ROOT})
set(DSP ${ROOT}/CMSIS/DSP)
list(APPEND CMAKE_MODULE_PATH ${DSP})
add_subdirectory("${DSP}/Source" bin_dsp)

function(compilerSpecificCompileOptions PROJECTNAME ROOT)
endfunction()
endif(BUILD_DSP_LIB)

#FREE
set(FREERTOS_PATH ${CMAKE_SOURCE_DIR}/FreeRTOS)

if(MINGW)
file(GLOB RTOS_SOURCE_FILE

    ${FREERTOS_PATH}/Source/portable/MSVC-MingW/port.c
    ${FREERTOS_PATH}/Source/portable/MemMang/heap_4.c
    ${FREERTOS_PATH}/Source/*.c
    ${FREERTOS_PATH}/Source/portable/Common/code_coverage_additions.c
    ${FREERTOS_PATH}/Source/portable/Common/Run-time-stats-utils.c

    )
include_directories(
    RTOS_INCLUDE
    ${CMAKE_SOURCE_DIR}/windows_sim
    ${FREERTOS_PATH}/Source/include
    ${FREERTOS_PATH}/Source/portable/MSVC-MingW
)
elseif(UNIX)

file(GLOB RTOS_SOURCE_FILE

${FREERTOS_PATH}/Source/*.c
        ${FREERTOS_PATH}/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
	${FREERTOS_PATH}/Source/portable/ThirdParty/GCC/Posix/port.c
	${FREERTOS_PATH}/Source/portable/MemMang/heap_4.c	
    )
include_directories(
    RTOS_INCLUDE
    ${FREERTOS_PATH}/Source
    ${FREERTOS_PATH}/Source/include
    ${FREERTOS_PATH}/Source/portable/ThirdParty/GCC/Posix
    ${FREERTOS_PATH}/Source/portable/ThirdParty/GCC/Posix/utils
    ${FREERTOS_PATH}/Source/portable/MemMang
    ${FREERTOS_PATH}/Source/portable
)
endif()


#build 


if(MINGW)

set(win_simulation simulation)
add_executable(${win_simulation} ${CMAKE_SOURCE_DIR}/windows_sim/main.c  ${RTOS_INCLUDE} ${RTOS_SOURCE_FILE})

target_compile_definitions(${win_simulation} PRIVATE
    -DprojCOVERAGE_TEST=1
    )

#add_executable()
target_link_libraries(
${win_simulation} PRIVATE
    winMM.Lib
)

elseif(UNIX)

set(linux_simulation simulation)
add_executable(${linux_simulation} ${CMAKE_SOURCE_DIR}/windows_sim/main.c  ${RTOS_INCLUDE} ${RTOS_SOURCE_FILE})

target_compile_definitions(${linux_simulation} PRIVATE
    -DprojCOVERAGE_TEST=1
    	)


#add_executable()
target_link_libraries(
${linux_simulation} PRIVATE
     pthread
    m
)

endif()





