set(ICODENTAL_SOURCES
    domain/CaseStatus.cpp
    domain/ProviderType.cpp
    domain/ImageFingerprint.cpp
    domain/AnalysisResult.cpp
    domain/CaseRecord.cpp
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

)

add_library(icodental_core STATIC ${ICODENTAL_SOURCES})

target_link_libraries(icodental_core
    PUBLIC 
        Qt6::Core
        Qt6::Sql
)

target_include_directories(icodental_core
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
