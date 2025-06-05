cmake_minimum_required(VERSION "3.25" FATAL_ERROR)

if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_policy(PUSH)
    cmake_policy(SET CMP0007 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0010 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0012 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0054 NEW)
    cmake_policy(PUSH)
    cmake_policy(SET CMP0057 NEW)
endif()

function(vscode)
    set(options)
    set(oneValueKeywords
        "SOURCE_DIR"
        "BINARY_DIR"

        "LAUNCH_GENERATE"
        "LAUNCH_FILE_OVERWRITE"
        "LAUNCH_TEMPLATE_FILE"
        "LAUNCH_FILE"
        "LAUNCH_TESTS_FILE"
        "LAUNCH_DEFAULT_TEST"
    )
    set(multiValueKeywords
        "LAUNCH_ENV_PATH"
        "LAUNCH_ADD_OPTIONS"
    )

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        unset("_${v}")
    endforeach()

    cmake_parse_arguments("" "${options}" "${oneValueKeywords}" "${multiValueKeywords}" "${ARGN}")

    if(NOT "${_UNPARSED_ARGUMENTS}" STREQUAL "")
        message(FATAL_ERROR "UNPARSED_ARGUMENTS: '${_UNPARSED_ARGUMENTS}'")
    endif()

    if("${_SOURCE_DIR}" STREQUAL "")
        cmake_path(GET "CMAKE_CURRENT_LIST_DIR" PARENT_PATH "_SOURCE_DIR")
    endif()

    if("${_BINARY_DIR}" STREQUAL "")
        set(_BINARY_DIR "-")
    endif()

    if("${_LAUNCH_GENERATE}" STREQUAL "")
        set(_LAUNCH_GENERATE "true")
    endif()

    if("${_LAUNCH_FILE_OVERWRITE}" STREQUAL "")
        set(_LAUNCH_FILE_OVERWRITE "true")
    endif()

    if("${_LAUNCH_TEMPLATE_FILE}" STREQUAL "")
        set(_LAUNCH_TEMPLATE_FILE "${_SOURCE_DIR}/src/test/resources/vscode/launch.json")
    endif()

    if("${_LAUNCH_FILE}" STREQUAL "")
        set(_LAUNCH_FILE "${_SOURCE_DIR}/.vscode/launch.json")
    endif()

    if("${_LAUNCH_TESTS_FILE}" STREQUAL "")
        set(_LAUNCH_TESTS_FILE "${_BINARY_DIR}/test/bin/tests.json")
    endif()

    if("${_LAUNCH_DEFAULT_TEST}" STREQUAL "")
        set(_LAUNCH_DEFAULT_TEST "-")
    endif()

    if("${_LAUNCH_ENV_PATH}" STREQUAL "")
        set(_LAUNCH_ENV_PATH "-")
    endif()

    if("${_LAUNCH_ADD_OPTIONS}" STREQUAL "")
        set(_LAUNCH_ADD_OPTIONS "-")
    endif()

    foreach(v IN LISTS "options" "oneValueKeywords" "multiValueKeywords")
        list(LENGTH "_${v}" l)
        if("${l}" GREATER "1")
            message(STATUS "${v}:")
            foreach(i IN LISTS "_${v}")
                message(STATUS "'${i}'")
            endforeach()
        else()
            message(STATUS "${v}: '${_${v}}'")
        endif()
    endforeach()

    set(checkNotEmptyVarNames
        "LAUNCH_GENERATE"
        "LAUNCH_FILE_OVERWRITE"
        "LAUNCH_FILE"
        "LAUNCH_TESTS_FILE"
        "LAUNCH_DEFAULT_TEST"

        "LAUNCH_ENV_PATH"
        "LAUNCH_ADD_OPTIONS"
    )

    foreach(v IN LISTS "checkNotEmptyVarNames")
        list(LENGTH "_${v}" l)
        if("${l}" GREATER "1")
            message(STATUS "${v}:")
            foreach(i IN LISTS "_${v}")
                if("${i}" STREQUAL "")
                    message(FATAL_ERROR "empty: '${i}'")
                endif()
            endforeach()
        else()
            if("${_${v}}" STREQUAL "")
                message(FATAL_ERROR "empty: '_${v}'")
            endif()
        endif()
    endforeach()

    set(checkExistsVarNames ${options} ${oneValueKeywords} ${multiValueKeywords})
    list(REMOVE_ITEM "checkExistsVarNames" ${checkNotEmptyVarNames})

    foreach(v IN LISTS "checkExistsVarNames")
        list(LENGTH "_${v}" l)
        if("${l}" GREATER "1")
            message(STATUS "${v}:")
            foreach(i IN LISTS "_${v}")
                if(NOT EXISTS "${i}")
                    message(FATAL_ERROR "not exists ${v}: '${i}'")
                endif()
            endforeach()
        else()
            if(NOT EXISTS "${_${v}}")
                message(FATAL_ERROR "not exists ${v}: '${_${v}}'")
            endif()
        endif()
    endforeach()

    if(
        NOT EXISTS "${_LAUNCH_TESTS_FILE}"
        OR NOT "${_LAUNCH_GENERATE}"
        OR (EXISTS "${_LAUNCH_FILE}" AND NOT "${_LAUNCH_FILE_OVERWRITE}")
    )
        return()
    endif()

    set(testCaseNames)
    if(NOT "${_LAUNCH_ADD_OPTIONS}" STREQUAL "")
        foreach(v IN LISTS "_LAUNCH_ADD_OPTIONS")
            list(APPEND "testCaseNames" "${v}")
        endforeach()
    endif()
    file(READ "${_LAUNCH_TESTS_FILE}" json)
    string(JSON testGroups GET "${json}" "testsuites")
    string(JSON testGroupsLength LENGTH "${testGroups}")
    if("${testGroupsLength}" GREATER "0")
        math(EXPR testGroupMaxIndex "${testGroupsLength} - 1")
        foreach(testGroupIndex RANGE "0" "${testGroupMaxIndex}")
            string(JSON testGroup GET "${testGroups}" "${testGroupIndex}")
            string(JSON testGroupName GET "${testGroup}" "name")
            string(JSON testCases GET "${testGroup}" "testsuite")
            string(JSON testCasesLength LENGTH "${testCases}")
            if("${testCasesLength}" GREATER "0")
                math(EXPR testCaseMaxIndex "${testCasesLength} - 1")
                foreach(testCaseIndex RANGE "0" "${testCaseMaxIndex}")
                    string(JSON testCase GET "${testCases}" "${testCaseIndex}")
                    string(JSON testCaseName GET "${testCase}" "name")
                    if(NOT "${testGroupName}.*" IN_LIST "testCaseNames")
                        list(APPEND testCaseNames "${testGroupName}.*")
                    endif()
                    if(NOT "${testGroupName}.${testCaseName}" IN_LIST "testCaseNames")
                        list(APPEND testCaseNames "${testGroupName}.${testCaseName}")
                    endif()
                endforeach()
            endif()
        endforeach()
    endif()

    cmake_path(RELATIVE_PATH "_BINARY_DIR" BASE_DIRECTORY "${_SOURCE_DIR}" OUTPUT_VARIABLE "binary-dir-rel")
    if("${_LAUNCH_ENV_PATH}" STREQUAL "-")
        set("env-path" "${binary-dir-rel}")
    else()
        cmake_path(CONVERT "${_LAUNCH_ENV_PATH}" TO_NATIVE_PATH_LIST "env-path" NORMALIZE)
        string(REPLACE "\\" "\\\\" "env-path" "${env-path}")
    endif()
    if("${_LAUNCH_DEFAULT_TEST}" STREQUAL "-")
        list(GET "testCaseNames" "0" "launch.inputs.pick-test.default")
    else()
        set("launch.inputs.pick-test.default" "${_LAUNCH_DEFAULT_TEST}")
    endif()
    string(JOIN "\",\n                \"" "launch.inputs.pick-test.options" ${testCaseNames})

    message(STATUS "substitutions:")
    message(STATUS "'@binary-dir-rel@': '${binary-dir-rel}'")
    message(STATUS "'@env-path@': '${env-path}'")
    message(STATUS "'@launch.inputs.pick-test.default@': '${launch.inputs.pick-test.default}'")
    message(STATUS "'@launch.inputs.pick-test.options@': '${launch.inputs.pick-test.options}'")

    configure_file("${_LAUNCH_TEMPLATE_FILE}" "${_LAUNCH_FILE}" @ONLY)

    message(STATUS "generated: '${_LAUNCH_FILE}'")
endfunction()

block()
    if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
        set(args)
        set(argsStarted "FALSE")
        math(EXPR argIndexMax "${CMAKE_ARGC} - 1")

        foreach(i RANGE "0" "${argIndexMax}")
            if("${argsStarted}")
                list(APPEND args "${CMAKE_ARGV${i}}")
            elseif(NOT "${argsStarted}" AND "${CMAKE_ARGV${i}}" STREQUAL "--")
                set(argsStarted "TRUE")
            endif()
        endforeach()

        set(noEscapeBackslashOption "--no-escape-backslash")

        if("${args}" STREQUAL "")
            cmake_path(GET CMAKE_CURRENT_LIST_FILE FILENAME fileName)
            message(FATAL_ERROR "Usage: cmake -P ${fileName} -- [${noEscapeBackslashOption}] function_name args...")
        endif()

        list(POP_FRONT args firstArg)
        set(escapeBackslash "TRUE")

        if("${firstArg}" STREQUAL "${noEscapeBackslashOption}")
            set(escapeBackslash "FALSE")
            list(POP_FRONT args func)
        else()
            set(func "${firstArg}")
        endif()

        set(wrappedArgs "")

        if(NOT "${args}" STREQUAL "")
            foreach(arg IN LISTS args)
                set(wrappedArg "${arg}")
                string(FIND "${wrappedArg}" " " firstIndexOfSpace)

                if(NOT "${firstIndexOfSpace}" EQUAL "-1")
                    set(wrappedArg "\"${wrappedArg}\"")
                endif()

                if("${escapeBackslash}")
                    string(REPLACE "\\" "\\\\" wrappedArg "${wrappedArg}")
                endif()

                if("${wrappedArgs}" STREQUAL "")
                    string(APPEND wrappedArgs "${wrappedArg}")
                else()
                    string(APPEND wrappedArgs " ${wrappedArg}")
                endif()
            endforeach()
        endif()

        cmake_language(EVAL CODE "${func}(${wrappedArgs})")
    endif()
endblock()

if(NOT "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "" AND "${CMAKE_SCRIPT_MODE_FILE}" STREQUAL "${CMAKE_CURRENT_LIST_FILE}")
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
    cmake_policy(POP)
endif()
