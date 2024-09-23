# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.22.0-rc2)

# We name the project and the target for the ExternalProject_Add() call
# to something that will highlight to the user what we are working on if
# something goes wrong and an error message is produced.

project(,-populate NONE)


# Pass through things we've already detected in the main project to avoid
# paying the cost of redetecting them again in ExternalProject_Add()
set(GIT_EXECUTABLE [==[/usr/local/bin/git]==])
set(GIT_VERSION_STRING [==[2.23.0]==])
set_property(GLOBAL PROPERTY _CMAKE_FindGit_GIT_EXECUTABLE_VERSION
  [==[/usr/local/bin/git;2.23.0]==]
)


include(ExternalProject)
ExternalProject_Add(,-populate
                     "UPDATE_DISCONNECTED" "False" "GIT_REPOSITORY" "https://github.com/xmos/," "GIT_TAG" "HEAD" "GIT_SHALLOW" "OFF"
                    SOURCE_DIR          "/Users/lsj/XMOS_XTC_15.3.0/examples/sw_usb_audio-_sw_v8_1_0/,"
                    BINARY_DIR          "/Users/lsj/XMOS_XTC_15.3.0/examples/sw_usb_audio-_sw_v8_1_0/sw_usb_audio/app_usb_aud_xk_216_mc/build/_deps/,-build"
                    CONFIGURE_COMMAND   ""
                    BUILD_COMMAND       ""
                    INSTALL_COMMAND     ""
                    TEST_COMMAND        ""
                    USES_TERMINAL_DOWNLOAD  YES
                    USES_TERMINAL_UPDATE    YES
)


