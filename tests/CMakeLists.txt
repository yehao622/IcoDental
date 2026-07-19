add_executable(icodental_filesystem_tests
    filesystem/FileSystemTests.cpp
)

target_link_libraries(icodental_filesystem_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Test
)

add_test(NAME icodental_filesystem_tests COMMAND icodental_filesystem_tests)

add_executable(icodental_cache_tests
    cache/CacheRepositoryTests.cpp
)

target_link_libraries(icodental_cache_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Sql
        Qt6::Test
)

add_test(NAME icodental_cache_tests COMMAND icodental_cache_tests)

add_executable(icodental_application_tests
    application/AnalysisOrchestratorTests.cpp
)

target_link_libraries(icodental_application_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Sql
        Qt6::Test
)

add_test(NAME icodental_application_tests COMMAND icodental_application_tests)

add_executable(icodental_provider_tests
    providers/ProviderModelsTests.cpp
)

target_link_libraries(icodental_provider_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Test
)

add_test(NAME icodental_provider_tests COMMAND icodental_provider_tests)

add_executable(icodental_gemini_payload_tests
    providers/GeminiPayloadBuilderTests.cpp
)

target_link_libraries(icodental_gemini_payload_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Test
)

add_test(NAME icodental_gemini_payload_tests COMMAND icodental_gemini_payload_tests)

add_executable(icodental_gemini_parser_tests
    providers/GeminiResponseParserTests.cpp
)

target_link_libraries(icodental_gemini_parser_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Test
)

add_test(NAME icodental_gemini_parser_tests COMMAND icodental_gemini_parser_tests)

add_executable(icodental_gemini_client_tests
    providers/GeminiClientTests.cpp
)

target_link_libraries(icodental_gemini_client_tests
    PRIVATE
        icodental_core
        Qt6::Core
        Qt6::Test
)

add_test(NAME icodental_gemini_client_tests COMMAND icodental_gemini_client_tests)