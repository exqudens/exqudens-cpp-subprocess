#include <cstddef>
#include <cstdlib>
#include <string>
#include <optional>
#include <vector>
#include <filesystem>
#include <stdexcept>
#include <iostream>

#include <gmock/gmock.h>
#include <gtest/gtest.h>

#include "TestApplication.hpp"
#include "TestUtils.hpp"

// include test files
#include "unit/SubProcessUnitTests.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"

int TestApplication::run(int argc, char** argv) {
    try {
        std::vector<std::string> args = {};
        for (int i = 0; i < argc; i++) {
            std::cout << "argv[" << i << "]: '" << argv[i] << "'" << std::endl;
            args.emplace_back(argv[i]);
        }

        TestUtils::init(args);

        testing::InitGoogleMock(&argc, argv);
        testing::InitGoogleTest(&argc, argv);

        int result = RUN_ALL_TESTS();

        return result;
    } catch (const std::exception& e) {
        std::cerr << CALL_INFO + ": " + TestUtils::toString(e) << std::endl;
        return EXIT_FAILURE;
    } catch (...) {
        std::cerr << CALL_INFO + ": Unknown error!" << std::endl;
        return EXIT_FAILURE;
    }
}

#undef CALL_INFO
