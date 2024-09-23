cmake_minimum_required(VERSION 3.21)
include($ENV{XMOS_CMAKE_PATH}/xcommon.cmake)
project(example)

set(APP_HW_TARGET XUF216-512-QF60-C20)

XMOS_REGISTER_APP()
