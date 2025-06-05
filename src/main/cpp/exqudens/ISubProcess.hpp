#pragma once

#include <cstddef>
#include <string>
#include <functional>

#include "exqudens/subprocess/export.hpp"

namespace exqudens {

    class EXQUDENS_SUBPROCESS_EXPORT ISubProcess {

        public:

            virtual std::string getVersion() = 0;

            virtual void setLogFunction(
                const std::function<void(
                    const std::string& file,
                    const size_t& line,
                    const std::string& function,
                    const std::string& id,
                    const unsigned short& level,
                    const std::string& message
                )>& value
            ) = 0;

            virtual bool isSetLogFunction() = 0;

            virtual void open(const std::string& command) = 0;

            virtual bool isOpen() = 0;

            virtual void write(const std::string& value) = 0;

            virtual std::string read() = 0;

            virtual void close() = 0;

            virtual ~ISubProcess() = default;

    };

}
