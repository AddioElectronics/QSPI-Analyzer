target_compile_definitions(QSPI_Analyzer
    PRIVATE
        KINGST_ANALYZER
)

set(AnalyzerSDK_DIR "${CMAKE_SOURCE_DIR}/thirdparty/KingstAnalyzerSDK")

message(STATUS "Targeting Kingst SDK: ${AnalyzerSDK_DIR}")

find_package(AnalyzerSDK CONFIG REQUIRED)

target_link_libraries(QSPI_Analyzer PUBLIC Kingst::AnalyzerSDK)