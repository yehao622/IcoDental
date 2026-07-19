set(ICODENTAL_SOURCES
    domain/CaseStatus.cpp
    domain/ProviderType.cpp
    domain/ImageFingerprint.cpp
    domain/AnalysisResult.cpp
    domain/CaseRecord.cpp
    application/AnalysisRequest.cpp
    application/AnalysisItem.cpp
    application/AnalysisPlan.cpp
    application/AnalysisOrchestrator.cpp
    infrastructure/config/ModelCatalog.cpp
    infrastructure/config/AppConfig.cpp
    infrastructure/config/SettingsRepository.cpp
    infrastructure/config/SecretResolver.cpp
    infrastructure/filesystem/SupportedImageTypes.cpp
    infrastructure/filesystem/FileScanner.cpp
    infrastructure/filesystem/ImageHasher.cpp
    infrastructure/cache/AnalysisCacheEntry.cpp
    infrastructure/cache/CacheDatabase.cpp
    infrastructure/cache/AnalysisCacheRepository.cpp
    infrastructure/providers/ProviderRequest.cpp
    infrastructure/providers/ProviderResponse.cpp
    infrastructure/providers/GeminiClient.cpp
    infrastructure/providers/GeminiPayloadBuilder.cpp
    infrastructure/providers/GeminiResponseParser.cpp
    infrastructure/providers/QtNetworkExecutor.cpp
)

add_library(icodental_core STATIC ${ICODENTAL_SOURCES})

target_link_libraries(icodental_core
    PUBLIC 
        Qt6::Core
        Qt6::Sql
        Qt6::Network
)

target_include_directories(icodental_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
