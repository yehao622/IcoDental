cmake_minimum_required(VERSION 3.24)
project(IcoDental VERSION 0.1.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)
set(CMAKE_AUTOMOC ON)

find_package(Qt6 REQUIRED COMPONENTS Core Sql Network Test)

option(ENABLE_COVERAGE "Enable code coverage instrumentation" OFF)

function(enable_coverage_for_target target_name)
    if(ENABLE_COVERAGE)
        if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
            target_compile_options(${target_name} PRIVATE -O0 -g --coverage)
            target_link_options(${target_name} PRIVATE --coverage)
        endif()
    endif()
endfunction()

if(ENABLE_COVERAGE)
    if(NOT CMAKE_BUILD_TYPE STREQUAL "Debug")
        message(WARNING "Coverage is most reliable with a Debug build.")
    endif()
    
    find_program(LCOV_EXECUTABLE lcov)
    find_program(GENHTML_EXECUTABLE genhtml)

    if(LCOV_EXECUTABLE AND GENHTML_EXECUTABLE)
        add_custom_target(coverage
            COMMAND ${CMAKE_COMMAND} -E echo "Resetting old coverage counters..."
            COMMAND find ${CMAKE_BINARY_DIR} -name "*.gcda" -delete

            COMMAND ${CMAKE_COMMAND} -E echo "Running tests..."
            COMMAND ${CMAKE_CTEST_COMMAND} --test-dir ${CMAKE_BINARY_DIR} --output-on-failure

            COMMAND ${CMAKE_COMMAND} -E echo "Capturing coverage..."
            COMMAND ${LCOV_EXECUTABLE}
                    --capture
                    --directory ${CMAKE_BINARY_DIR}
                    --output-file ${CMAKE_BINARY_DIR}/coverage.info

            COMMAND ${CMAKE_COMMAND} -E echo "Filtering coverage..."
            COMMAND ${LCOV_EXECUTABLE}
                    --remove ${CMAKE_BINARY_DIR}/coverage.info
                    '/usr/*'
                    '*/${CMAKE_BINARY_DIR_NAME}/*_autogen/*'
                    '*/tests/*'
                    --output-file ${CMAKE_BINARY_DIR}/coverage.filtered.info

            COMMAND ${CMAKE_COMMAND} -E echo "Generating HTML report..."
            COMMAND ${GENHTML_EXECUTABLE}
                    ${CMAKE_BINARY_DIR}/coverage.filtered.info
                    --output-directory ${CMAKE_BINARY_DIR}/coverage-html

            WORKING_DIRECTORY ${CMAKE_SOURCE_DIR}
            COMMENT "Running tests and generating coverage report"
        )
    else()
        message(WARNING "lcov/genhtml not found; coverage target will not be available")
    endif()
endif()

add_subdirectory(src)

enable_testing()
add_subdirectory(tests)