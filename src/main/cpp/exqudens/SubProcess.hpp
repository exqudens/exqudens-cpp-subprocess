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
            bool processOpen = false;
            void* ptrParentForWrite = nullptr;
            void* ptrParentForRead = nullptr;
            void* ptrChildIn = nullptr;
            void* ptrChildOut = nullptr;
            void* ptrChildProcess = nullptr;
            void* ptrChildThread = nullptr;

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

            void close() override;

            ~SubProcess() override;

    };

}
