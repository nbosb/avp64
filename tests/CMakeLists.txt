##############################################################################
#                                                                            #
# Copyright 2024 Lukas Jünger, Nils Bosbach                                  #
#                                                                            #
# This software is licensed under the MIT license.                           #
# A copy of the license can be found in the LICENSE file at the root         #
# of the source tree.                                                        #
#                                                                            #
##############################################################################

set(CMAKE_EXE_LINKER_FLAGS -rdynamic)
find_package(Python3 "3.9" REQUIRED)

get_target_property(ld_library_path ocx-qemu-arm BINARY_DIR)

add_library(test_main test_main.cpp)
target_link_libraries(test_main PUBLIC gtest)
target_link_libraries(test_main PUBLIC avp64)
target_link_libraries(test_main PUBLIC vcml)

macro(new_test test)
    add_executable(${test} ${test}.cpp)
    target_include_directories(${test} PRIVATE ${inc} ${SYSTEMC_INCLUDE_DIRS})
    target_link_libraries(${test} test_main)
    target_compile_options(${test} PRIVATE ${MWR_COMPILER_WARN_FLAGS})
    add_test(NAME ${test} COMMAND ${test} ${CMAKE_CURRENT_SOURCE_DIR})
    set_tests_properties(${test} PROPERTIES TIMEOUT 5)
    set_tests_properties(${test} PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH})
endmacro()

new_test(arm64_core_test)
if (AVP64_BUILD_RUNNER)
    function(linux_boot nrcpu config timeout)
        set(name "linux-boot-${nrcpu}-cpus")
        set(config ${CMAKE_SOURCE_DIR}/sw/${config})
        set(script ${CMAKE_CURRENT_BINARY_DIR}/${name})
        if (CMAKE_CONFIGURATION_TYPES)
            configure_file(linux_boot.py.in ${script}.py.in)
            file(GENERATE OUTPUT ${script}-$<CONFIG>.py INPUT ${script}.py.in)
            add_test(NAME ${name} COMMAND ${Python3_EXECUTABLE} ${script}-$<CONFIG>.py)
        else()
            configure_file(linux_boot.py.in ${script}.py)
            file(GENERATE OUTPUT ${script}.py INPUT ${script}.py)
            add_test(NAME ${name} COMMAND ${Python3_EXECUTABLE} ${script}.py)
        endif()
        set_tests_properties(${name} PROPERTIES TIMEOUT ${timeout})
        set_tests_properties(${name} PROPERTIES ENVIRONMENT LD_LIBRARY_PATH=${ld_library_path}:$ENV{LD_LIBRARY_PATH})
    endfunction()

    linux_boot(1 buildroot_6_5_6-x1.cfg 600)

    string(TOUPPER ${CMAKE_BUILD_TYPE} BUILD_TYPE_UPPER)
    if(BUILD_TYPE_UPPER STREQUAL RELEASE)
        linux_boot(2 buildroot_6_5_6-x2.cfg 600)
        linux_boot(4 buildroot_6_5_6-x4.cfg 600)
        linux_boot(8 buildroot_6_5_6-x8.cfg 600)
    endif()
endif()
