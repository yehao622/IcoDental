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