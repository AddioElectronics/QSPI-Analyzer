target_compile_definitions(QSPI_Analyzer 
    PRIVATE 
        SALEAE_ANALYZER
)

set(AnalyzerSDK_DIR "${CMAKE_SOURCE_DIR}/thirdparty/SaleaeAnalyzerSDK")

message(STATUS "Targeting Saleae SDK: ${AnalyzerSDK_DIR}")

# SDK uses SYSTEM_PROCESSOR to detect arch
# For windows they expect 'ARM64', 
# when 'arm64' is what it actually is.
if(CMAKE_GENERATOR_PLATFORM STREQUAL "arm64")
    set(CMAKE_SYSTEM_PROCESSOR "ARM64")
endif()

find_package(AnalyzerSDK CONFIG REQUIRED)

target_link_libraries(QSPI_Analyzer PUBLIC Saleae::AnalyzerSDK)