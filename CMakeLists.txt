cmake_minimum_required(VERSION 3.14...3.22)

project(QSPI_Analyzer VERSION 1.0.1 LANGUAGES CXX)
add_library(QSPI_Analyzer SHARED)
target_compile_features(QSPI_Analyzer PRIVATE cxx_std_17)

set_target_properties(QSPI_Analyzer PROPERTIES
    OUTPUT_NAME "QSPI_Analyzer"
    PREFIX ""
)

target_compile_features(QSPI_Analyzer PRIVATE cxx_std_17)

set(BUILD_SHARED_LIBS ON) # Dynamic library

target_sources(QSPI_Analyzer PRIVATE
    src/QSpiAnalyzer.cpp
    src/QSpiAnalyzer.h
    src/QSpiAnalyzerResults.cpp
    src/QSpiAnalyzerResults.h
    src/QSpiAnalyzerSettings.cpp
    src/QSpiAnalyzerSettings.h
    src/QSpiDataChannelManager.cpp
    src/QSpiDataChannelManager.h
    src/QSpiSimulationDataGenerator.cpp
    src/QSpiSimulationDataGenerator.h
    src/QspiTypes.h
    src/QSpiUtil.h
)

# Select SDK
if(NOT DEFINED TARGET_SDK OR TARGET_SDK STREQUAL "saleae")
    include(cmake/Saleae.cmake)
elseif(TARGET_SDK MATCHES "kingst")
    include(cmake/Kingst.cmake)
endif()