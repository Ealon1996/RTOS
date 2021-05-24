cmake_minimum_required(VERSION 3.14.0)



project(dsptest)
set(CMAKE_BUILD_TYPE DEBUG)
SET (CMAKE_C_FLAGS "${CMAKE_C_FLAGS} -O2 -ggdb  -m32 -g -UUSE_STDIO")


# options
option(USE_RTOS_CLI OFF)
option(BUILD_ALGO_LIB OFF)
option(BUILD_DSPTEST OFF)
option(BUILD_PEAKTEST OFF)
file(GLOB ALGO_SROURCES
    algo/*.c
    )


file(GLOB TASK_SOURCES
    task/*.c
    )


if(USE_RTOS_CLI)
    file(GLOB RTOS_PLUS_SROURCES
        FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI/FreeRTOS_CLI.c

        )

endif()


file(GLOB RTOS_SROURCES
	FreeRTOS/Source/*.c
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils/wait_for_event.c
	FreeRTOS/Source/portable/ThirdParty/GCC/Posix/port.c
	FreeRTOS/Source/portable/MemMang/heap_4.c
        ${RTOS_PLUS_SROURCES}
)


set(ROOT /home/ed/CMSIS_5)
set(DSP ${ROOT}/CMSIS/DSP)
list(APPEND CMAKE_MODULE_PATH ${DSP})
add_subdirectory("${DSP}/Source" bin_dsp)

if(BUILD_ALGO_LIB)

add_library(
    ALGO_LIB STATIC

    ${ALGO_SROURCES}
    )

target_compile_definitions(ALGO_LIB PRIVATE
         -DDEBUG=1
        -D__GNUC_PYTHON__
        -D__GCC_POSIX__=1
        -DUSE_OS=1
        -DPLATFORM=1
        -DprojCOVERAGE_TEST=1
    )
target_include_directories(
        ALGO_LIB PRIVATE
        include/dsp/
        include/
        algo
        FreeRTOS
        FreeRTOS/Source
        FreeRTOS/Source/include
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils
        FreeRTOS/Source/portable/MemMang
        FreeRTOS/Source/portable
        FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI
        task/
    )
target_link_libraries(
    ALGO_LIB
    CMSISDSP
)

endif()


if(BUILD_DSPTEST)
set(ALGO_TEST algo.out)
add_executable(${ALGO_TEST} main.c  ${RTOS_SROURCES} ${TASK_SOURCES})
target_compile_definitions(${ALGO_TEST} PRIVATE
         -DDEBUG=1
         -DprojCOVERAGE_TEST=1
    )
target_include_directories(${ALGO_TEST} PRIVATE
        include/dsp/
        include/
        algo
        FreeRTOS
        FreeRTOS/Source
        FreeRTOS/Source/include
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils
        FreeRTOS/Source/portable/MemMang
        FreeRTOS/Source/portable
        FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI
        task/

)

target_link_libraries(
    ${ALGO_TEST}
    ALGO_LIB
    CMSISDSP
   pthread
    m
)
endif()

# build for unitest
if(BUILD_PEAKTEST)

set(PEAKTEST peaktest.out)
add_executable(${PEAKTEST} ${CMAKE_SOURCE_DIR}/unitest/peaktest.c  ${RTOS_SROURCES} ${TASK_SOURCES})

target_compile_definitions(${PEAKTEST} PRIVATE
         -DDEBUG=1
         -DprojCOVERAGE_TEST=1
)

target_include_directories(${PEAKTEST} PRIVATE
        include/dsp/
        include/
        algo
        FreeRTOS
        FreeRTOS/Source
        FreeRTOS/Source/include
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix
        FreeRTOS/Source/portable/ThirdParty/GCC/Posix/utils
        FreeRTOS/Source/portable/MemMang
        FreeRTOS/Source/portable
        FreeRTOS-Plus/Source/FreeRTOS-Plus-CLI
        task/

)

target_link_libraries(
    ${PEAKTEST}
    ALGO_LIB
    CMSISDSP
   pthread
    m
)

endif()

