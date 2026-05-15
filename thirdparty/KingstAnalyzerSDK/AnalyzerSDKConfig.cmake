# Notice: This file is not included with the Kingst AnalyzerSDK.
#         Do not delete if upgrading to a newer SDK version.

add_library(Kingst::AnalyzerSDK SHARED IMPORTED)

set_target_properties(Kingst::AnalyzerSDK PROPERTIES
    INTERFACE_INCLUDE_DIRECTORIES ${CMAKE_CURRENT_LIST_DIR}/inc
)

# -----------------------------
# macOS
# -----------------------------
if(APPLE)

    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        message(FATAL_ERROR "Kingst SDK on macOS only supports x86_64")
    endif()

    set_target_properties(Kingst::AnalyzerSDK PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/Mac/libAnalyzer.dylib
    )

# -----------------------------
# Windows
# -----------------------------
elseif(WIN32)
    if(CMAKE_GENERATOR_PLATFORM STREQUAL "ARM64")
        message(FATAL_ERROR "Kingst SDK does not support Windows ARM64")

    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "Win32")
        set(LIB_DIR "lib/Win32")

    elseif(CMAKE_GENERATOR_PLATFORM STREQUAL "x64" OR
           CMAKE_SYSTEM_PROCESSOR MATCHES "AMD64|x86_64")
        set(LIB_DIR "lib/Win64")

    else()
        message(FATAL_ERROR "Unsupported Windows architecture: ${CMAKE_GENERATOR_PLATFORM}")
    endif()

    set_target_properties(Kingst::AnalyzerSDK PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/${LIB_DIR}/Analyzer.dll
        IMPORTED_IMPLIB   ${CMAKE_CURRENT_LIST_DIR}/${LIB_DIR}/Analyzer.lib
    )

# -----------------------------
# Linux
# -----------------------------
elseif(UNIX)

    if(NOT CMAKE_SYSTEM_PROCESSOR MATCHES "x86_64|AMD64")
        message(FATAL_ERROR "Kingst SDK on Linux only supports x86_64")
    endif()

    set_target_properties(Kingst::AnalyzerSDK PROPERTIES
        IMPORTED_LOCATION ${CMAKE_CURRENT_LIST_DIR}/lib/Linux/libAnalyzer.so
        IMPORTED_SONAME   libAnalyzer.so
    )

# -----------------------------
# Unsupported platform
# -----------------------------
else()
    message(FATAL_ERROR "Unsupported platform for Kingst SDK")
endif()