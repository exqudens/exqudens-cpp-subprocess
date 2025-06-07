#pragma once

#include <exqudens/ISubProcess.hpp>

namespace exqudens {

    class EXQUDENS_SUBPROCESS_EXPORT SubProcess : public ISubProcess {

        private:

            std::function<void(
                const std::string& file,
                const size_t& line,
                const std::string& function,
                const std::string& id,
                const unsigned short& level,
                const std::string& message
            )> logFunction;
            bool autoClose = true;
            void* parentIn = nullptr;
            void* parentOut = nullptr;
            void* parentErr = nullptr;
            void* childIn = nullptr;
            void* childOut = nullptr;
            void* childErr = nullptr;
            void* childProcess = nullptr;
            void* childThread = nullptr;

        public:

            SubProcess(bool autoClose);
            SubProcess();

            std::string getVersion() override;

            void setLogFunction(
                const std::function<void(
                    const std::string& file,
                    const size_t& line,
                    const std::string& function,
                    const std::string& id,
                    const unsigned short& level,
                    const std::string& message
                )>& value
            ) override;

            bool isSetLogFunction() override;

            void open(const std::string& command) override;

            bool isOpen() override;

            void write(const std::string& value) override;

            std::string read() override;

            std::string readError() override;

            unsigned long close() override;

            ~SubProcess() override;

        private:

            std::string readInternal(bool out);

    };

}
