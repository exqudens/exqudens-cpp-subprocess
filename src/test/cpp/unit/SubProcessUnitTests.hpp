#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestUtils.hpp"
#include "exqudens/SubProcess.hpp"

#define LOG_LEVEL_FATAL 1
#define LOG_LEVEL_ERROR 2
#define LOG_LEVEL_WARNING 3
#define LOG_LEVEL_INFO 4
#define LOG_LEVEL_DEBUG 5
#define LOG_LEVEL_TRACE 6

namespace exqudens {

    class SubProcessUnitTests: public testing::Test {

        protected:

            inline static const char* LOGGER_ID = "exqudens.SubProcessUnitTests";

        protected:

            static void log(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            ) {
                if (level > 0 && level <= 6) {
                    std::string internalLevel = "NONE";
                    if (level == 1) {
                        internalLevel = "FATAL";
                    } else if (level == 2) {
                        internalLevel = "ERROR";
                    } else if (level == 3) {
                        internalLevel = "WARNING";
                    } else if (level == 4) {
                        internalLevel = "INFO";
                    } else if (level == 5) {
                        internalLevel = "DEBUG";
                    } else if (level == 6) {
                        internalLevel = "TRACE";
                    }
                    std::string internalFile = std::filesystem::path(file).filename().string();
                    std::string row = {};
                    row += internalLevel;
                    row += " [";
                    row += id;
                    row += "] ";
                    row += function;
                    row += "(";
                    row += internalFile;
                    row += ":";
                    row += std::to_string(line);
                    row += "): ";
                    row += '"';
                    row += message;
                    row += '"';
                    std::cout << row << std::endl;
                }
            }

    };

    TEST_F(SubProcessUnitTests, test_1) {
        try {
            std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
            std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, "bgn");

            std::string command = {};
            std::shared_ptr<ISubProcess> interface = {};
            std::string version = {};
            std::string data = {};

            command = std::filesystem::path(std::filesystem::path(TestUtils::getProjectBinaryDir()) / "test" / "bin" / "test-executable.exe").generic_string();
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("command: ") + command);

            ASSERT_TRUE(std::filesystem::exists(std::filesystem::path(command)));

            command = std::string("\"") + command + '"';
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("command: ") + command);

            interface = std::make_shared<SubProcess>();
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("interface: ") + std::to_string((bool) interface));

            interface->setLogFunction(&SubProcessUnitTests::log);
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("interface.isSetLogFunction: ") + std::to_string(interface->isSetLogFunction()));

            version = interface->getVersion();
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("version: ") + version);

            interface->open(command);

            data = "aaa";
            interface->write(data);
            data = interface->read();
            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, std::string("data: ") + data);

            interface->close();

            log(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOG_LEVEL_INFO, "end");
        } catch (const std::exception& e) {
            std::string errorMessage = TestUtils::toString(e);
            std::cout << LOGGER_ID << " ERROR: " << errorMessage << std::endl;
            FAIL() << errorMessage;
        }
    }

}

#undef LOG_LEVEL_TRACE
#undef LOG_LEVEL_DEBUG
#undef LOG_LEVEL_INFO
#undef LOG_LEVEL_WARNING
#undef LOG_LEVEL_ERROR
#undef LOG_LEVEL_FATAL
