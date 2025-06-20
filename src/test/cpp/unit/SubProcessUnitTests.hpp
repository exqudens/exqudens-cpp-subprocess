#pragma once

#include <cstddef>
#include <string>
#include <vector>
#include <memory>
#include <filesystem>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>
#include <exqudens/Log.hpp>
#include <exqudens/log/api/Logging.hpp>

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

            inline static const char* LOGGER_ID = "SubProcessUnitTests";

        protected:

            static void log(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            ) {
                exqudens::log::api::Logging::Writer(file, line, function, id, level) << message;
            }

    };

    TEST_F(SubProcessUnitTests, test_1) {
        try {
            std::string testGroup = testing::UnitTest::GetInstance()->current_test_info()->test_suite_name();
            std::string testCase = testing::UnitTest::GetInstance()->current_test_info()->name();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "bgn";

            std::string command = {};
            std::shared_ptr<ISubProcess> interface = {};
            std::string version = {};
            std::string data = {};
            std::string expected = {};
            std::string actual = {};
            unsigned long expectedExitCode = {};
            unsigned long actualExitCode = {};

            command = std::filesystem::path(std::filesystem::path(TestUtils::getProjectBinaryDir()) / "test" / "bin" / "test-executable.exe").generic_string();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "command: '" << command << "'";

            ASSERT_TRUE(std::filesystem::exists(std::filesystem::path(command)));

            command = std::string("\"") + command + '"';
            EXQUDENS_LOG_INFO(LOGGER_ID) << "command: '" << command << "'";

            interface = std::make_shared<SubProcess>();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "interface: " << (bool) interface;

            interface->setLogFunction(&SubProcessUnitTests::log);
            EXQUDENS_LOG_INFO(LOGGER_ID) << "interface.isSetLogFunction: " << interface->isSetLogFunction();

            version = interface->getVersion();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "version: '" << version << "'";

            // case-1
            interface->open(command);

            interface->write("stdout\r\n");

            data = "abc123\r\n";
            EXQUDENS_LOG_INFO(LOGGER_ID) << "data: '" << data << "'";
            interface->write(data);

            expected = TestUtils::trim(TestUtils::toUpper(data));
            EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";

            data = interface->read();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "data: '" << data << "'";

            actual = TestUtils::trim(data);
            EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

            ASSERT_EQ(expected, actual);

            data = "exit\r\n";
            interface->write(data);

            expectedExitCode = 0;
            actualExitCode = interface->close();

            ASSERT_EQ(expectedExitCode, actualExitCode);

            // case-2
            interface->open(command);

            interface->write("stderr\r\n");

            data = "321cba\r\n";
            EXQUDENS_LOG_INFO(LOGGER_ID) << "data: '" << data << "'";
            interface->write(data);

            expected = TestUtils::trim(TestUtils::toUpper(data));
            EXQUDENS_LOG_INFO(LOGGER_ID) << "expected: '" << expected << "'";

            data = interface->readError();
            EXQUDENS_LOG_INFO(LOGGER_ID) << "data: '" << data << "'";

            actual = TestUtils::trim(data);
            EXQUDENS_LOG_INFO(LOGGER_ID) << "actual: '" << actual << "'";

            ASSERT_EQ(expected, actual);

            data = "error-2\r\n";
            interface->write(data);

            expectedExitCode = 2;
            actualExitCode = interface->close();

            ASSERT_EQ(expectedExitCode, actualExitCode);

            EXQUDENS_LOG_INFO(LOGGER_ID) << "end";
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
