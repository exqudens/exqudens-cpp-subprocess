cmake_minimum_required(VERSION 3.25 FATAL_ERROR)

set(CMAKE_UTIL_FILE "" CACHE STRING "...")
set(USE_CONAN "1" CACHE BOOL "...")

message(STATUS "USE_CONAN: '${USE_CONAN}'")

if("${USE_CONAN}")
    if("windows" IN_LIST "PRESET_NAME_ELEMENTS")
        find_program(CONAN_COMMAND NAMES "conan.exe" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    else()
        find_program(CONAN_COMMAND NAMES "conan" PATHS ENV CONAN_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
    endif()
endif()

if("${CMAKE_UTIL_FILE}" STREQUAL "")
    if("$ENV{CMAKE_UTIL_FILE}" STREQUAL "")
        if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake")
            if("${USE_CONAN}")
                #[[execute_process(
                    COMMAND "${CONAN_COMMAND}" "user" "$ENV{CONAN_LOGIN_USERNAME}" "-r" "artifactory" "-p"
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL "ANY"
                )]]
                execute_process(
                    COMMAND "${CONAN_COMMAND}" "install" "--tool-requires=exqudens-cmake/2.0.1" "--deployer" "direct_deploy" "--deployer-folder" "${CMAKE_SOURCE_DIR}/build/dependencies"
                    COMMAND_ECHO "STDERR"
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL "ANY"
                )
                set(CMAKE_UTIL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake" CACHE STRING "..." FORCE)
            else()
                message(FATAL_ERROR "Unable to resolve CMAKE_UTIL_FILE: '${CMAKE_UTIL_FILE}'")
            endif()
        else()
            set(CMAKE_UTIL_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/dependencies/direct_deploy/exqudens-cmake/cmake/util.cmake" CACHE STRING "..." FORCE)
        endif()
    else()
        set(CMAKE_UTIL_FILE "$ENV{CMAKE_UTIL_FILE}" CACHE STRING "..." FORCE)
    endif()
endif()

set(SKIP_TEST "0" CACHE BOOL "...")
set(TEST_GROUP ".*UnitTests" CACHE STRING "...")
set(TEST_CASE "all" CACHE STRING "...")

message(STATUS "CMAKE_UTIL_FILE: '${CMAKE_UTIL_FILE}'")
message(STATUS "SKIP_TEST: '${SKIP_TEST}'")
message(STATUS "TEST_GROUP: '${TEST_GROUP}'")
message(STATUS "TEST_CASE: '${TEST_CASE}'")

include("${CMAKE_UTIL_FILE}")
include("${CMAKE_SOURCE_DIR}/cmake/util.cmake")

string(REPLACE "." ";" PRESET_NAME_ELEMENTS "${PRESET_NAME}")
if("msvc-x64-x64" IN_LIST "PRESET_NAME_ELEMENTS")
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/msvc-x64-x64-toolchain.cmake")
        set_msvc_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            VERSION "16"
            HOST "x64"
            TARGET "x64"
            OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/msvc-x64-x64-toolchain.cmake"
        )
    endif()
elseif("gcc" IN_LIST PRESET_NAME_ELEMENTS)
    if(NOT EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/build/gcc-toolchain.cmake")
        find_file(COMPILER_FILE NAMES "gcc" PATHS ENV GCC_PATH ENV PATH REQUIRED NO_CACHE NO_DEFAULT_PATH)
        set_gnu_toolchain(TOOLCHAIN_CONTENT
            PROCESSOR "x86_64"
            OS "${CMAKE_HOST_SYSTEM_NAME}"
            PATH "${COMPILER_FILE}"
            OUTPUT_FILE "${CMAKE_CURRENT_SOURCE_DIR}/build/gcc-toolchain.cmake"
        )
    endif()
endif()

message(STATUS "CMAKE_TOOLCHAIN_FILE: '${CMAKE_TOOLCHAIN_FILE}'")

file(READ "${CMAKE_CURRENT_SOURCE_DIR}/name-version.txt" INFO)
string(REPLACE ":" ";" INFO_LIST "${INFO}")
list(GET INFO_LIST 0 RAW_NAME)
list(GET INFO_LIST 1 RAW_VERSION)
string(STRIP "${RAW_NAME}" NAME)
string(STRIP "${RAW_VERSION}" VERSION)
project("${NAME}" VERSION "${VERSION}" LANGUAGES "NONE")

message(STATUS "PROJECT_NAME: '${PROJECT_NAME}'")
message(STATUS "PROJECT_VERSION: '${PROJECT_VERSION}'")

string(REPLACE "_" "-" BASE_DIR "${PROJECT_NAME}")
string(REPLACE "-cpp-" "-" BASE_DIR "${BASE_DIR}")
string(REPLACE "-" "/" BASE_DIR "${BASE_DIR}")

message(STATUS "BASE_DIR: '${BASE_DIR}'")

string(REPLACE "/" "_" BASE_NAME "${BASE_DIR}")
string(TOUPPER "${BASE_NAME}" BASE_NAME)

message(STATUS "BASE_NAME: '${BASE_NAME}'")

if("$ENV{CLION_IDE}")
    cmake_path(CONVERT "$ENV{PATH}" TO_CMAKE_PATH_LIST TMP_CMAKE_ENV_PATH NORMALIZE)
    list(REMOVE_AT TMP_CMAKE_ENV_PATH 0)
    cmake_path(CONVERT "${TMP_CMAKE_ENV_PATH}" TO_NATIVE_PATH_LIST TMP_NATIVE_ENV_PATH NORMALIZE)
    set("ENV{PATH}" "${TMP_NATIVE_ENV_PATH}")
endif()

enable_language("C")
enable_language("CXX")
include("GenerateExportHeader")
enable_testing()
include("GoogleTest")

if("${CMAKE_INSTALL_PREFIX_INITIALIZED_TO_DEFAULT}")
    set(CMAKE_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/cmake-install" CACHE PATH "..." FORCE)
endif()
set(CMAKE_VERBOSE_MAKEFILE "TRUE" CACHE BOOL "" FORCE)
set(CMAKE_EXPORT_COMPILE_COMMANDS "TRUE" CACHE BOOL "" FORCE)

cmake_path(RELATIVE_PATH "PROJECT_BINARY_DIR" BASE_DIRECTORY "${PROJECT_SOURCE_DIR}" OUTPUT_VARIABLE "PROJECT_BINARY_DIR_REL")
cmake_path(CONVERT "$ENV{PATH}" TO_CMAKE_PATH_LIST "ENV_PATH" NORMALIZE)
set(CMAKE_OBJECT_PATH_MAX 1000)
set(CMAKE_SHARED_LIBRARY_PREFIX "")
set(CMAKE_SHARED_MODULE_PREFIX "")
set(CMAKE_STATIC_LIBRARY_PREFIX "")
set(CMAKE_IMPORT_LIBRARY_PREFIX "")
set(CMAKE_STAGING_PREFIX "")
set(CMAKE_FIND_LIBRARY_PREFIXES "" "lib")
set(CMAKE_FIND_USE_CMAKE_ENVIRONMENT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_SYSTEM_PATH FALSE)
set(CMAKE_FIND_USE_SYSTEM_ENVIRONMENT_PATH TRUE)
set(CMAKE_FIND_USE_SYSTEM_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_REGISTRY FALSE)
set(CMAKE_FIND_USE_PACKAGE_ROOT_PATH FALSE)
set(CMAKE_FIND_USE_CMAKE_PATH TRUE)
set(CMAKE_FIND_PACKAGE_PREFER_CONFIG FALSE)
set(CMAKE_BUILD_RPATH "\$ORIGIN")
set(CMAKE_INSTALL_RPATH "\$ORIGIN")
set(CMAKE_SKIP_BUILD_RPATH TRUE)
set(CMAKE_BUILD_RPATH_USE_ORIGIN TRUE)
set(CMAKE_BUILD_WITH_INSTALL_RPATH TRUE)
set(CMAKE_INSTALL_RPATH_USE_LINK_PATH FALSE)
set(CMAKE_WINDOWS_EXPORT_ALL_SYMBOLS FALSE)
set(CMAKE_CXX_VISIBILITY_PRESET "hidden")
set(CMAKE_VISIBILITY_INLINES_HIDDEN TRUE)
set(CMAKE_C_STANDARD 11)
set(CMAKE_C_STANDARD_REQUIRED TRUE)
set(CMAKE_C_EXTENSIONS FALSE)
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)
set(CMAKE_CXX_EXTENSIONS FALSE)

separate_arguments(CMAKE_CXX_FLAGS NATIVE_COMMAND "${CMAKE_CXX_FLAGS}")
if(MSVC)
    if(NOT "/EHa" IN_LIST CMAKE_CXX_FLAGS AND "/EHsc" IN_LIST CMAKE_CXX_FLAGS)
        list(REMOVE_ITEM CMAKE_CXX_FLAGS "/EHsc")
        list(APPEND CMAKE_CXX_FLAGS "/EHa")
    endif()
endif()
string(JOIN " " CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if("${BUILD_SHARED_LIBS}")
    set(CONAN_RELEASE_COMPILER_RUNTIME "MD")
else()
    set(CONAN_RELEASE_COMPILER_RUNTIME "MT")
    if("${MINGW}")
        set(CMAKE_CXX_STANDARD_LIBRARIES "-static-libgcc -static-libstdc++ ${CMAKE_CXX_STANDARD_LIBRARIES}")
        set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} -Wl,-Bstatic,--whole-archive -lwinpthread -Wl,--no-whole-archive")
    endif()
endif()

set(TARGET_CMAKE_INSTALL_DEPENDS_ON "${PROJECT_NAME}")
if(NOT "${SKIP_TEST}")
    set(TARGET_CMAKE_INSTALL_DEPENDS_ON "cmake-test")
    if("${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\..+$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\..+$")
    elseif("${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^.+\\.${TEST_CASE}$")
    elseif(NOT "${TEST_GROUP}" STREQUAL "all" AND NOT "${TEST_CASE}" STREQUAL "all")
        set(TEST_REGEXP "^${TEST_GROUP}\\.${TEST_CASE}$")
    endif()

    message(STATUS "TEST_GROUP: '${TEST_GROUP}'")
    message(STATUS "TEST_CASE: '${TEST_CASE}'")
    message(STATUS "TEST_REGEXP: '${TEST_REGEXP}'")
endif()

message(STATUS "TARGET_CMAKE_INSTALL_DEPENDS_ON:")
foreach(v IN LISTS "TARGET_CMAKE_INSTALL_DEPENDS_ON")
    message(STATUS "${v}")
endforeach()

set(CONAN_INSTALL_PREFIX "${PROJECT_BINARY_DIR}/conan-install")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/lib")

file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/test/bin")
file(MAKE_DIRECTORY "${CONAN_INSTALL_PREFIX}/test/lib")

if("${USE_CONAN}")
    set_conan_architecture(CONAN_ARCH "${CMAKE_SYSTEM_PROCESSOR}")
    set_conan_compiler(CONAN_COMPILER "${CMAKE_CXX_COMPILER_ID}")
    set_conan_compiler_version(CONAN_COMPILER_VERSION "${CMAKE_CXX_COMPILER_ID}" "${CMAKE_CXX_COMPILER_VERSION}" MAX_ELEMENTS "1")
    set_conan_compiler_runtime(CONAN_COMPILER_RUNTIME "${CMAKE_MSVC_RUNTIME_LIBRARY}")
    set_conan_compiler_runtime_type(CONAN_COMPILER_RUNTIME_TYPE "${CMAKE_MSVC_RUNTIME_LIBRARY}")

    set_python_boolean(CONAN_INTERFACE_ONLY "${INTERFACE_ONLY}")
    set_python_boolean(CONAN_BUILD_SHARED_LIBS "${BUILD_SHARED_LIBS}")

    set_conan_settings(CONAN_SETTINGS
        "arch=${CONAN_ARCH}"
        "os=${CMAKE_SYSTEM_NAME}"
        "compiler=${CONAN_COMPILER}"
        "compiler.version=${CONAN_COMPILER_VERSION}"
        "compiler.cstd=${CMAKE_C_STANDARD}"
        "compiler.cppstd=${CMAKE_CXX_STANDARD}"
        "compiler.runtime=${CONAN_COMPILER_RUNTIME}"
        "compiler.runtime_type=${CONAN_COMPILER_RUNTIME_TYPE}"
        "compiler.libcxx=${CONAN_COMPILER_LIBCXX}"
        "build_type=${CMAKE_BUILD_TYPE}"
    )
    set_conan_options(CONAN_OPTIONS
        "&:shared=${CONAN_BUILD_SHARED_LIBS}"
        "*:shared=${CONAN_BUILD_SHARED_LIBS}"
    )
    if(NOT "${SKIP_TEST}")
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
            execute_process(
                #[[execute_process(
                    COMMAND "${CONAN_COMMAND}" "user" "$ENV{CONAN_LOGIN_USERNAME}" "-r" "artifactory" "-p"
                    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                    ENCODING "UTF-8"
                    COMMAND_ERROR_IS_FATAL "ANY"
                )]]
                COMMAND "${CONAN_COMMAND}" "install"
                        "--output-folder" "${CONAN_INSTALL_PREFIX}/test"
                        ${CONAN_SETTINGS}
                        ${CONAN_OPTIONS}
                        "${PROJECT_SOURCE_DIR}/test-conanfile.py"
                COMMAND_ECHO "STDERR"
                ENCODING "UTF-8"
                COMMAND_ERROR_IS_FATAL "ANY"
            )
        endif()
        if(NOT EXISTS "${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
            message(FATAL_ERROR "Not exists: '${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake'")
        endif()
        include("${CONAN_INSTALL_PREFIX}/test/conan-packages.cmake")
        list(APPEND CMAKE_MODULE_PATH "${CONAN_INSTALL_PREFIX}/test")
        list(APPEND CMAKE_PREFIX_PATH "${CONAN_INSTALL_PREFIX}/test")
        foreach(
            conanPackageName
            cmakePackageName
            cmakePackageVersion
            cmakePackagePath
            IN ZIP_LISTS
            "${PROJECT_NAME}_CONAN_PACKAGE_NAMES"
            "${PROJECT_NAME}_CMAKE_PACKAGE_NAMES"
            "${PROJECT_NAME}_CMAKE_PACKAGE_VERSIONS"
            "${PROJECT_NAME}_CMAKE_PACKAGE_PATHS"
        )
            if(
                "" STREQUAL "${conanPackageName}"
                OR "" STREQUAL "${cmakePackageName}"
                OR "" STREQUAL "${cmakePackageVersion}"
                OR "" STREQUAL "${cmakePackagePath}"
            )
                string(JOIN " " errorMessage
                    "Empty value 'conanPackageName': '${conanPackageName}'"
                    "or 'cmakePackageName': '${cmakePackageName}'"
                    "or 'cmakePackageVersion': '${cmakePackageVersion}'"
                    "or 'cmakePackagePath': '${cmakePackagePath}'!"
                )
                message(FATAL_ERROR "${errorMessage}")
            elseif(NOT IS_DIRECTORY "${cmakePackagePath}")
                message(FATAL_ERROR "Not is directory 'cmakePackagePath': '${cmakePackagePath}'!")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION" "${cmakePackageVersion}")
            endif()

            if("${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH}" STREQUAL "")
                set("${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_PATH" "${cmakePackagePath}")
            endif()

            list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES" "${cmakePackageName}")
            list(APPEND CMAKE_PREFIX_PATH "${cmakePackagePath}")
        endforeach()
    endif()
endif()

if("${${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES}" STREQUAL "")
    #[[list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
        "dependecy"
    )]]
    if(NOT "${SKIP_TEST}")
        list(APPEND "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES"
            "GTest"
            "exqudens-cpp-log"
        )
    endif()
endif()
message(STATUS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES:")
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    message(STATUS "${cmakePackageName}")
endforeach()
unset(NOT_FOUND_PACKAGE_NAMES)
foreach(cmakePackageName IN LISTS "${PROJECT_NAME}_CMAKE_FIND_PACKAGE_NAMES")
    if("GTest" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    elseif("exqudens-cpp-log" STREQUAL "${cmakePackageName}")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}Config.cmake")
        file(REMOVE "${CONAN_INSTALL_PREFIX}/test/${cmakePackageName}-config.cmake")
        list(APPEND NOT_FOUND_PACKAGE_NAMES "${cmakePackageName}")
        find_package("${cmakePackageName}" "${${PROJECT_NAME}_CMAKE_PACKAGE_${cmakePackageName}_VERSION}" EXACT QUIET)
    else()
        message(WARNING "Ignore cmakePackageName: '${cmakePackageName}'")
    endif()
endforeach()
set_not_found_package_names(NOT_FOUND_PACKAGE_NAMES ${NOT_FOUND_PACKAGE_NAMES})
if(NOT "" STREQUAL "${NOT_FOUND_PACKAGE_NAMES}")
    foreach(package IN LISTS NOT_FOUND_PACKAGE_NAMES)
        message(SEND_ERROR "${package}_FOUND: '${${package}_FOUND}' ${package}_NOT_FOUND_MESSAGE: '${${package}_NOT_FOUND_MESSAGE}'")
    endforeach()
    message(FATAL_ERROR "Some package not found!")
endif()

configure_file("${PROJECT_SOURCE_DIR}/versions.hpp.in" "${PROJECT_BINARY_DIR}/generated/src/main/cpp/${BASE_DIR}/versions.hpp" @ONLY NEWLINE_STYLE UNIX)

add_library("${PROJECT_NAME}"
    "src/main/cpp/exqudens/ISubProcess.hpp"
    "src/main/cpp/exqudens/SubProcess.hpp"
    "src/main/cpp/exqudens/SubProcess.cpp"
)
set(INLINE_MACRO_NAME "${BASE_NAME}_INLINE")
set(INLINE_MACRO "#  define ${INLINE_MACRO_NAME}")
#[[if("${INTERFACE_ONLY}")
    set(INLINE_MACRO "#  define ${INLINE_MACRO_NAME} inline")
endif()]]
string(MAKE_C_IDENTIFIER "${PROJECT_NAME}_EXPORTS" EXPORTS_MACRO_NAME)
string(JOIN "\n" EXPORT_HEADER_CUSTOM_CONTENT
    ""
    "#ifndef ${INLINE_MACRO_NAME}"
    "${INLINE_MACRO}"
    "#endif /* ${INLINE_MACRO_NAME} */"
    ""
)
generate_export_header("${PROJECT_NAME}"
    BASE_NAME "${BASE_NAME}"
    EXPORT_FILE_NAME "generated/src/main/cpp/${BASE_DIR}/export.hpp"
    CUSTOM_CONTENT_FROM_VARIABLE "EXPORT_HEADER_CUSTOM_CONTENT"
)
if(NOT "${BUILD_SHARED_LIBS}")
    target_compile_definitions("${PROJECT_NAME}" PUBLIC
        "${BASE_NAME}_STATIC_DEFINE"
    )
endif()
target_include_directories("${PROJECT_NAME}" PUBLIC
    "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/main/cpp>"
    "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/main/cpp>"
    "$<INSTALL_INTERFACE:include>"
)
set_target_properties("${PROJECT_NAME}" PROPERTIES
    RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/bin"
    RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/bin"

    ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
    ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"

    LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/main/lib"
    LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/main/lib"
)
#[[target_link_libraries("${PROJECT_NAME}" PUBLIC
    "dependecy::dependecy"
)]]
#set_property(TARGET "${PROJECT_NAME}" PROPERTY "VERSION" "${PROJECT_VERSION}")
#set_property(TARGET "${PROJECT_NAME}" PROPERTY "SOVERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" PROPERTY "INTERFACE_${PROJECT_NAME}_MAJOR_VERSION" "${PROJECT_VERSION}")
set_property(TARGET "${PROJECT_NAME}" APPEND PROPERTY "COMPATIBLE_INTERFACE_STRING" "${PROJECT_NAME}_MAJOR_VERSION")
install(
    EXPORT "${PROJECT_NAME}-targets"
    DESTINATION "cmake"
    NAMESPACE "${PROJECT_NAME}::"
    FILE "${PROJECT_NAME}-targets.cmake"
)
include(CMakePackageConfigHelpers)
configure_package_config_file(
    "${PROJECT_SOURCE_DIR}/package-config.cmake.in"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    INSTALL_DESTINATION "cmake"
)
write_basic_package_version_file(
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    VERSION "${PROJECT_VERSION}"
    COMPATIBILITY "ExactVersion"
)
install(
    FILES "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config.cmake"
    "${PROJECT_BINARY_DIR}/${PROJECT_NAME}-config-version.cmake"
    DESTINATION "cmake"
)
install(
    DIRECTORY "${PROJECT_BINARY_DIR}/generated/src/main/cpp/"
              "src/main/cpp/"
    DESTINATION "include"
    FILES_MATCHING
    PATTERN "*.h"
    PATTERN "*.hpp"
)
install(
    TARGETS "${PROJECT_NAME}"
    EXPORT "${PROJECT_NAME}-targets"
    INCLUDES DESTINATION "include"
    RUNTIME DESTINATION "bin"
    LIBRARY DESTINATION "lib"
    ARCHIVE DESTINATION "lib"
)

if(NOT "${SKIP_TEST}")
    add_executable("test-executable"
        "src/test/cpp/test_executable_main.cpp"
    )
    set_target_properties("test-executable" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
    )

    add_library("test-lib"
        "src/test/cpp/TestUtils.hpp"
        "src/test/cpp/TestUtils.cpp"
        "src/test/cpp/TestApplication.hpp"
        "src/test/cpp/TestApplication.cpp"
        "src/test/cpp/unit/SubProcessUnitTests.hpp"
    )
    generate_export_header("test-lib"
        BASE_NAME "TEST_LIB"
        EXPORT_FILE_NAME "generated/src/test/cpp/test_lib_export.hpp"
    )
    target_include_directories("test-lib" PUBLIC
        "$<BUILD_INTERFACE:${PROJECT_SOURCE_DIR}/src/test/cpp>"
        "$<BUILD_INTERFACE:${PROJECT_BINARY_DIR}/generated/src/test/cpp>"
        "$<INSTALL_INTERFACE:include>"
    )
    target_link_libraries("test-lib" PUBLIC
        "${PROJECT_NAME}"
        "GTest::gmock"
        "GTest::gtest"
        "GTest::gmock_main"
        "GTest::gtest_main"
        "exqudens-cpp-log::exqudens-cpp-log"
    )
    if(NOT "${BUILD_SHARED_LIBS}")
        target_compile_definitions("test-lib" PUBLIC
            "TEST_LIB_STATIC_DEFINE"
        )
    endif()
    set_target_properties("test-lib" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
    )

    add_executable("test-app"
        "src/test/cpp/main.cpp"
    )
    target_link_libraries("test-app"
        "test-lib"
    )
    set_target_properties("test-app" PROPERTIES
        RUNTIME_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/bin"
        RUNTIME_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/bin"

        ARCHIVE_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        ARCHIVE_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"

        LIBRARY_OUTPUT_DIRECTORY                "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELEASE        "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_RELWITHDEBINFO "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_MINSIZEREL     "${PROJECT_BINARY_DIR}/test/lib"
        LIBRARY_OUTPUT_DIRECTORY_DEBUG          "${PROJECT_BINARY_DIR}/test/lib"
    )
    add_dependencies("test-app" "test-executable")
    if("${BUILD_SHARED_LIBS}")
        if("${CMAKE_SYSTEM_NAME}" STREQUAL "Windows")
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},RUNTIME_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/test/bin" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        else()
            add_custom_command(TARGET "test-app"
                PRE_BUILD
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "$<TARGET_PROPERTY:${PROJECT_NAME},LIBRARY_OUTPUT_DIRECTORY>" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                #COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                COMMAND "${CMAKE_COMMAND}" -E copy_directory "${CONAN_INSTALL_PREFIX}/test/lib" "$<TARGET_PROPERTY:test-app,RUNTIME_OUTPUT_DIRECTORY>"
                WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
                USES_TERMINAL
                VERBATIM
            )
        endif()
    endif()
    add_custom_command(TARGET "test-app"
        POST_BUILD
        COMMAND "$<TARGET_FILE:test-app>" "--gtest_list_tests" "--gtest_output=json:tests.json" "--project-binary-dir=${PROJECT_BINARY_DIR}"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:test-app>"
        USES_TERMINAL
        VERBATIM
    )

    gtest_discover_tests("test-app"
        DISCOVERY_EXTRA_ARGS "--project-binary-dir=${PROJECT_BINARY_DIR}"
        EXTRA_ARGS "--project-binary-dir=${PROJECT_BINARY_DIR}"
        WORKING_DIRECTORY "$<TARGET_FILE_DIR:test-app>"
        PROPERTIES ENVIRONMENT "PATH=$<TARGET_FILE_DIR:test-app>;${ENV_PATH}"
    )

    add_custom_command(
        OUTPUT "${PROJECT_BINARY_DIR}/junit.xml"
        COMMAND "${CMAKE_CTEST_COMMAND}" "--build-nocmake" "--output-junit" "${PROJECT_BINARY_DIR}/junit.xml" "--preset" "${PRESET_NAME}" "-R" "${TEST_REGEXP}"
        DEPENDS "test-app"
        WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
        USES_TERMINAL
        VERBATIM
    )

    add_custom_target("cmake-test"
        DEPENDS "${PROJECT_BINARY_DIR}/junit.xml"
    )
endif()

add_custom_command(
    OUTPUT "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}-config.cmake"
    COMMAND "${CMAKE_COMMAND}" --install "${PROJECT_BINARY_DIR}" --prefix "${CMAKE_INSTALL_PREFIX}"
    DEPENDS ${TARGET_CMAKE_INSTALL_DEPENDS_ON}
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("cmake-install" ALL
    DEPENDS "${CMAKE_INSTALL_PREFIX}/cmake/${PROJECT_NAME}-config.cmake"
)

add_custom_target("conan-export"
    COMMAND "${CONAN_COMMAND}" "export-pkg"
            "--output-folder" "${CMAKE_INSTALL_PREFIX}"
            ${CONAN_SETTINGS}
            ${CONAN_OPTIONS}
            #"--user" "gitlab-group+gitlab-sub-group+${PROJECT_NAME}"
            #"--channel" "stable"
            "${PROJECT_SOURCE_DIR}/conanfile.py"
    DEPENDS "cmake-install"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("conan-remove"
    COMMAND "${CONAN_COMMAND}" "remove" "-c" "${PROJECT_NAME}/${PROJECT_VERSION}"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("conan-list"
    COMMAND "${CONAN_COMMAND}" "list" "${PROJECT_NAME}/${PROJECT_VERSION}:*"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

add_custom_target("vscode"
    COMMAND "${CMAKE_COMMAND}" "-P" "cmake/util.cmake" "--" "vscode"
            SOURCE_DIR "${PROJECT_SOURCE_DIR}"
            BINARY_DIR "${PROJECT_BINARY_DIR}"

            LAUNCH_GENERATE "1"
            LAUNCH_FILE_OVERWRITE "1"
            LAUNCH_TEMPLATE_FILE "${PROJECT_SOURCE_DIR}/src/test/resources/vscode/launch.json"
            LAUNCH_FILE "${PROJECT_SOURCE_DIR}/.vscode/launch.json"
            LAUNCH_TESTS_FILE "${PROJECT_BINARY_DIR}/test/bin/tests.json"
            LAUNCH_DEFAULT_TEST "SubProcessUnitTests.test_1"
    DEPENDS "${PROJECT_NAME}" "test-app"
    WORKING_DIRECTORY "${PROJECT_SOURCE_DIR}"
    USES_TERMINAL
    VERBATIM
)

set_target_names(ALL_TARGETS "${CMAKE_CURRENT_SOURCE_DIR}")
message(STATUS "all-targets:")
foreach(t IN LISTS ALL_TARGETS)
    message(STATUS "${t}")
endforeach()

set("${PROJECT_NAME}_ADDITIONAL_CLEAN_FILES"
    "${CMAKE_INSTALL_PREFIX}"
)
set_directory_properties(PROPERTIES
    ADDITIONAL_CLEAN_FILES "${${PROJECT_NAME}_ADDITIONAL_CLEAN_FILES}"
)
