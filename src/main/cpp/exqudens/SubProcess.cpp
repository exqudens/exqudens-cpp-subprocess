#include <stdexcept>
#include <filesystem>

#include <windows.h>

#include "exqudens/SubProcess.hpp"
#include "exqudens/subprocess/versions.hpp"

#define CALL_INFO std::string(__FUNCTION__) + "(" + std::filesystem::path(__FILE__).filename().string() + ":" + std::to_string(__LINE__) + ")"
#define LOGGER_ID "exqudens.SubProcess"
#define LOGGER_LEVEL_ERROR 2
#define LOGGER_LEVEL_DEBUG 5

namespace exqudens {

    SubProcess::SubProcess(bool autoClose): autoClose(autoClose) {
    }

    SubProcess::SubProcess(): SubProcess(true) {
    }

    std::string SubProcess::getVersion() {
        try {
            std::string result = {};
            result += std::to_string(PROJECT_VERSION_MAJOR);
            result += ".";
            result += std::to_string(PROJECT_VERSION_MINOR);
            result += ".";
            result += std::to_string(PROJECT_VERSION_PATCH);
            if (logFunction) logFunction(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_DEBUG, result);
            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::setLogFunction(
        const std::function<void(
            const std::string& file,
            const size_t& line,
            const std::string& function,
            const std::string& id,
            const unsigned short& level,
            const std::string& message
        )>& value
    ) {
        try {
            logFunction = value;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool SubProcess::isSetLogFunction() {
        try {
            return (bool) logFunction;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::open(const std::string& command) {
        try {
            // Create pipes
            SECURITY_ATTRIBUTES saAttr;
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;

            if (!CreatePipe(&ptrParentForRead, &ptrChildOut, &saAttr, 0)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error creating 'out' pipe: " + std::to_string(GetLastError()));
            }
            if (!CreatePipe(&ptrChildIn, &ptrParentForWrite, &saAttr, 0)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error creating 'in' pipe: " + std::to_string(GetLastError()));
            }

            // Make sure that the pipe handles are not inherited by the child process
            SetHandleInformation(ptrChildOut, HANDLE_FLAG_INHERIT, 0);
            SetHandleInformation(ptrChildIn, HANDLE_FLAG_INHERIT, 0);

            // Set up STARTUPINFO
            STARTUPINFO si;
            ZeroMemory(&si, sizeof(STARTUPINFO));
            si.cb = sizeof(STARTUPINFO);
            si.hStdInput = ptrChildIn;
            si.hStdOutput = ptrChildOut;
            si.hStdError = ptrChildOut;
            si.dwFlags |= STARTF_USESTDHANDLES;

            // Create the process
            PROCESS_INFORMATION pi;
            ZeroMemory(&pi, sizeof(PROCESS_INFORMATION));

            if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), NULL, NULL, TRUE, 0, NULL, NULL, &si, &pi)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error creating process: " + std::to_string(GetLastError()));
            }

            // Store process poiters
            ptrChildProcess = pi.hProcess;
            ptrChildThread = pi.hThread;

            // Close unused handles
            /* CloseHandle(ptrChildIn);
            ptrChildIn = nullptr; */
            /* CloseHandle(ptrChildOut);
            ptrChildOut = nullptr; */

            // Wait for process to finish
            WaitForSingleObject(ptrChildProcess, INFINITE);

            processOpen = true;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool SubProcess::isOpen() {
        try {
            return processOpen;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::write(const std::string& value) {
        try {
            unsigned long bytesWritten = 0;
            if (!WriteFile(ptrParentForWrite, value.c_str(), value.size(), &bytesWritten, NULL)) {
                throw std::runtime_error(CALL_INFO + ": Error writing to process: " + std::to_string(GetLastError()));
            }
            if (bytesWritten != value.size()) {
                throw std::runtime_error(CALL_INFO + ": Error writing to process: bytesWritten: " + std::to_string(bytesWritten) + " != value.size: " + std::to_string(value.size()));
            }

            // Wait for process to finish
            WaitForSingleObject(ptrChildProcess, INFINITE);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::string SubProcess::read() {
        try {
            // Wait for process to finish
            WaitForSingleObject(ptrChildProcess, INFINITE);

            char buffer[1024];
            unsigned long bytesRead = 0;
            std::string output;
            while (ReadFile(/* ptrParentForRead */ ptrChildOut, buffer, sizeof(buffer) - 1, &bytesRead, NULL) && bytesRead > 0) {
                buffer[bytesRead] = '\0';
                output += buffer;
            }
            return output;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::close() {
        try {
            if (processOpen) {
                if (ptrParentForWrite) {
                    CloseHandle(ptrParentForWrite);
                    ptrParentForWrite = nullptr;
                }
                if (ptrParentForRead) {
                    CloseHandle(ptrParentForRead);
                    ptrParentForRead = nullptr;
                }
                if (ptrChildIn) {
                    CloseHandle(ptrChildIn);
                    ptrChildIn = nullptr;
                }
                if (ptrChildOut) {
                    CloseHandle(ptrChildOut);
                    ptrChildOut = nullptr;
                }
                if (ptrChildProcess) {
                    CloseHandle(ptrChildProcess);
                    ptrChildProcess = nullptr;
                }
                if (ptrChildThread) {
                    CloseHandle(ptrChildThread);
                    ptrChildThread = nullptr;
                }
                processOpen = false;
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    SubProcess::~SubProcess() {
        try {
            close();
        } catch (const std::exception& e) {
            if (logFunction) logFunction(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, std::string("throw in 'close': ") + e.what());
        } catch (...) {
            if (logFunction) logFunction(__FILE__, __LINE__, __FUNCTION__, LOGGER_ID, LOGGER_LEVEL_ERROR, "throw in 'close': unknown cause");
        }
    }

}

#undef LOGGER_LEVEL_DEBUG
#undef LOGGER_LEVEL_ERROR
#undef LOGGER_ID
#undef CALL_INFO
