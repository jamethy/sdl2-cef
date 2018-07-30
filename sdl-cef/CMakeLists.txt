
# Load the SDL libraries
find_package(SDL2 REQUIRED)

# Copyright (c) 2014 The Chromium Embedded Framework Authors. All rights
# reserved. Use of this source code is governed by a BSD-style license that
# can be found in the LICENSE file.

find_package(SDL2 REQUIRED)
include_directories(${SDL2_INCLUDE_DIR})

#
# Source files.
#

# cefsimple sources.
set(SDLCEF_SRCS
        simple_app.cc
        simple_app.h
        simple_handler.cc
        simple_handler.h
        sdl_cef_render_handler.cc
        sdl_cef_render_handler.h
        )
set(SDLCEF_SRCS_LINUX
        cefsimple_linux.cc
        simple_handler_linux.cc
        )
set(SDLCEF_SRCS_MACOSX
        cefsimple_mac.mm
        simple_handler_mac.mm
        )
set(SDLCEF_SRCS_WINDOWS
        cefsimple.exe.manifest
        cefsimple.rc
        cefsimple_win.cc
        resource.h
        simple_handler_win.cc
        )
APPEND_PLATFORM_SOURCES(SDLCEF_SRCS)
source_group(cefsimple FILES ${SDLCEF_SRCS})

set(SDLCEF_RES_SRCS_WINDOWS
        res/cefsimple.ico
        res/small.ico
        )
APPEND_PLATFORM_SOURCES(SDLCEF_RES_SRCS)
source_group(cefsimple\\\\res FILES ${SDLCEF_RES_SRCS})

set(SDLCEF_SRCS
        ${SDLCEF_SRCS}
        ${SDLCEF_RES_SRCS}
        )

# cefsimple helper sources.
set(SDLCEF_HELPER_SRCS_MACOSX
        process_helper_mac.cc
        )
APPEND_PLATFORM_SOURCES(SDLCEF_HELPER_SRCS)
source_group(cefsimple FILES ${SDLCEF_HELPER_SRCS})

# cefsimple resources.
set(SDLCEF_RESOURCES_MAC_SRCS_MACOSX
        mac/Info.plist
        mac/cefsimple.icns
        )
APPEND_PLATFORM_SOURCES(SDLCEF_RESOURCES_MAC_SRCS)
source_group(cefsimple\\\\mac FILES ${SDLCEF_RESOURCES_MAC_SRCS})

set(SDLCEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS_MACOSX
        mac/English.lproj/InfoPlist.strings
        mac/English.lproj/MainMenu.xib
        )
APPEND_PLATFORM_SOURCES(SDLCEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS)
source_group(cefsimple\\\\mac\\\\English.lproj FILES ${SDLCEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS})

set(SDLCEF_RESOURCES_SRCS
        ${SDLCEF_RESOURCES_MAC_SRCS}
        ${SDLCEF_RESOURCES_MAC_ENGLISH_LPROJ_SRCS}
        )


#
# Shared configuration.
#

# Target executable names.
set(SDLCEF_TARGET "sdlcef")
if(OS_MACOSX)
    set(CEF_HELPER_TARGET "sdlcef_Helper")
    set(CEF_HELPER_OUTPUT_NAME "sdlcef Helper")
endif()

# Logical target used to link the libcef library.
ADD_LOGICAL_TARGET("libcef_lib" "${CEF_LIB_DEBUG}" "${CEF_LIB_RELEASE}")

# Determine the target output directory.
SET_CEF_TARGET_OUT_DIR()


#
# Linux configuration.
#

if(OS_LINUX)
    # Executable target.
    add_executable(${SDLCEF_TARGET} ${SDLCEF_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${SDLCEF_TARGET})
    add_dependencies(${SDLCEF_TARGET} libcef_dll_wrapper)
    target_link_libraries(${SDLCEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS} ${SDL2_LIBRARY})

    # Set rpath so that libraries can be placed next to the executable.
    set_target_properties(${SDLCEF_TARGET} PROPERTIES INSTALL_RPATH "$ORIGIN")
    set_target_properties(${SDLCEF_TARGET} PROPERTIES BUILD_WITH_INSTALL_RPATH TRUE)
    set_target_properties(${SDLCEF_TARGET} PROPERTIES RUNTIME_OUTPUT_DIRECTORY ${CEF_TARGET_OUT_DIR})

    # Copy binary and resource files to the target output directory.
    COPY_FILES("${SDLCEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    COPY_FILES("${SDLCEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")

    # Set SUID permissions on the chrome-sandbox target.
    SET_LINUX_SUID_PERMISSIONS("${SDLCEF_TARGET}" "${CEF_TARGET_OUT_DIR}/chrome-sandbox")
endif()


#
# Mac OS X configuration.
#

if(OS_MACOSX)
    # Output paths for the app bundles.
    set(CEF_APP "${CEF_TARGET_OUT_DIR}/${SDLCEF_TARGET}.app")
    set(CEF_HELPER_APP "${CEF_TARGET_OUT_DIR}/${CEF_HELPER_OUTPUT_NAME}.app")

    # Variable referenced from Info.plist files.
    set(PRODUCT_NAME "${SDLCEF_TARGET}")

    # Helper executable target.
    add_executable(${CEF_HELPER_TARGET} MACOSX_BUNDLE ${SDLCEF_HELPER_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${CEF_HELPER_TARGET})
    add_dependencies(${CEF_HELPER_TARGET} libcef_dll_wrapper)
    target_link_libraries(${CEF_HELPER_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${CEF_HELPER_TARGET} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/helper-Info.plist
            OUTPUT_NAME ${CEF_HELPER_OUTPUT_NAME}
            )

    # Fix the framework rpath in the helper executable.
    FIX_MACOSX_HELPER_FRAMEWORK_RPATH(${CEF_HELPER_TARGET})

    # Main executable target.
    add_executable(${SDLCEF_TARGET} MACOSX_BUNDLE ${SDLCEF_RESOURCES_SRCS} ${SDLCEF_SRCS})
    SET_EXECUTABLE_TARGET_PROPERTIES(${SDLCEF_TARGET})
    add_dependencies(${SDLCEF_TARGET} libcef_dll_wrapper "${CEF_HELPER_TARGET}")
    target_link_libraries(${SDLCEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})
    set_target_properties(${SDLCEF_TARGET} PROPERTIES
            MACOSX_BUNDLE_INFO_PLIST ${CMAKE_CURRENT_SOURCE_DIR}/mac/Info.plist
            )

    # Copy files into the main app bundle.
    add_custom_command(
            TARGET ${SDLCEF_TARGET}
            POST_BUILD
            # Copy the helper app bundle into the Frameworks directory.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_HELPER_APP}"
            "${CEF_APP}/Contents/Frameworks/${CEF_HELPER_OUTPUT_NAME}.app"
            # Copy the CEF framework into the Frameworks directory.
            COMMAND ${CMAKE_COMMAND} -E copy_directory
            "${CEF_BINARY_DIR}/Chromium Embedded Framework.framework"
            "${CEF_APP}/Contents/Frameworks/Chromium Embedded Framework.framework"
            VERBATIM
    )

    # Fix the framework rpath in the main executable.
    FIX_MACOSX_MAIN_FRAMEWORK_RPATH(${SDLCEF_TARGET})

    # Manually process and copy over resource files.
    # The Xcode generator can support this via the set_target_properties RESOURCE
    # directive but that doesn't properly handle nested resource directories.
    # Remove these prefixes from input file paths.
    set(PREFIXES "mac/")
    COPY_MACOSX_RESOURCES("${SDLCEF_RESOURCES_SRCS}" "${PREFIXES}" "${SDLCEF_TARGET}" "${CMAKE_CURRENT_SOURCE_DIR}" "${CEF_APP}")
endif()


#
# Windows configuration.
#

if(OS_WINDOWS)
    # Executable target.
    add_executable(${SDLCEF_TARGET} WIN32 ${SDLCEF_SRCS})
    add_dependencies(${SDLCEF_TARGET} libcef_dll_wrapper)
    SET_EXECUTABLE_TARGET_PROPERTIES(${SDLCEF_TARGET})
    target_link_libraries(${SDLCEF_TARGET} libcef_lib libcef_dll_wrapper ${CEF_STANDARD_LIBS})

    if(USE_SANDBOX)
        # Logical target used to link the cef_sandbox library.
        ADD_LOGICAL_TARGET("cef_sandbox_lib" "${CEF_SANDBOX_LIB_DEBUG}" "${CEF_SANDBOX_LIB_RELEASE}")
        target_link_libraries(${SDLCEF_TARGET} cef_sandbox_lib ${CEF_SANDBOX_STANDARD_LIBS})
    endif()

    # Add the custom manifest files to the executable.
    ADD_WINDOWS_MANIFEST("${CMAKE_CURRENT_SOURCE_DIR}" "${SDLCEF_TARGET}" "exe")

    # Copy binary and resource files to the target output directory.
    COPY_FILES("${SDLCEF_TARGET}" "${CEF_BINARY_FILES}" "${CEF_BINARY_DIR}" "${CEF_TARGET_OUT_DIR}")
    COPY_FILES("${SDLCEF_TARGET}" "${CEF_RESOURCE_FILES}" "${CEF_RESOURCE_DIR}" "${CEF_TARGET_OUT_DIR}")
endif()
