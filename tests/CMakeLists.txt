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