#include <vector>
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
            if (isOpen()) {
                throw std::runtime_error(CALL_INFO + ": use 'close' first");
            }

            if (command.empty()) {
                throw std::runtime_error(CALL_INFO + ": empty 'command'");
            }

            // Create pipes
            SECURITY_ATTRIBUTES saAttr;
            saAttr.nLength = sizeof(SECURITY_ATTRIBUTES);
            saAttr.bInheritHandle = TRUE;
            saAttr.lpSecurityDescriptor = NULL;

            if (!CreatePipe(&parentOut, &childOut, &saAttr, 0) || !CreatePipe(&childIn, &parentIn, &saAttr, 0)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error creating pipes: " + std::to_string(GetLastError()));
            }

            // Set info
            if (!SetHandleInformation(parentIn, HANDLE_FLAG_INHERIT, 0) || !SetHandleInformation(parentOut, HANDLE_FLAG_INHERIT, 0)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error setting info: " + std::to_string(GetLastError()));
            }

            // Set up STARTUPINFO
            STARTUPINFO si;
            ZeroMemory(&si, sizeof(STARTUPINFO));
            si.cb = sizeof(si);
            si.hStdInput = childIn;
            si.hStdOutput = childOut;
            si.hStdError = childOut;
            si.dwFlags |= STARTF_USESTDHANDLES;

            // Create the process
            PROCESS_INFORMATION pi;
            ZeroMemory(&pi, sizeof(pi));

            // Create process
            if (!CreateProcess(NULL, const_cast<char*>(command.c_str()), nullptr, nullptr, TRUE, 0, nullptr, nullptr, &si, &pi)) {
                close();
                throw std::runtime_error(CALL_INFO + ": Error creating process: " + std::to_string(GetLastError()));
            }

            // Store process poiters
            childProcess = pi.hProcess;
            childThread = pi.hThread;

            // Close unused handles
            /* if (childIn) {
                CloseHandle(childIn);
                childIn = nullptr;
            } */
            /* if (childOut) {
                CloseHandle(childOut);
                childOut = nullptr;
            } */

            // Wait for process to finish
            //WaitForSingleObject(childProcess, INFINITE);
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    bool SubProcess::isOpen() {
        try {
            if (
                parentIn
                || parentOut
                || childIn
                || childOut
                || childProcess
                || childThread
            ) {
                return true;
            }
            return false;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::write(const std::string& value) {
        try {
            if (!isOpen()) {
                throw std::runtime_error(CALL_INFO + ": use 'open' first");
            }

            if (value.empty()) {
                return;
            }

            unsigned long bytesWritten = 0;

            if (!WriteFile(parentIn, value.c_str(), value.size(), &bytesWritten, NULL)) {
                throw std::runtime_error(CALL_INFO + ": Error writing to process: " + std::to_string(GetLastError()));
            }

            if (bytesWritten != value.size()) {
                throw std::runtime_error(CALL_INFO + ": Error writing to process: bytesWritten: " + std::to_string(bytesWritten) + " != value.size: " + std::to_string(value.size()));
            }
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    std::string SubProcess::read() {
        try {
            if (!isOpen()) {
                throw std::runtime_error(CALL_INFO + ": use 'open' first");
            }

            std::vector<char> buffer = {};
            std::string data = {};
            unsigned long bytesRead = 0;
            std::string result = {};

            while (true) {
                buffer = std::vector<char>(1024, 0);
                data = {};

                if (!ReadFile(parentOut, buffer.data(), buffer.size(), &bytesRead, nullptr) || bytesRead == 0 || buffer.back() == 0) {
                    data = std::string(buffer.data());
                    result += data;
                    break;
                }

                data = std::string(buffer.data());
                result += data;
            }

            return result;
        } catch (...) {
            std::throw_with_nested(std::runtime_error(CALL_INFO));
        }
    }

    void SubProcess::close() {
        try {
            if (isOpen()) {
                if (parentIn) {
                    CloseHandle(parentIn);
                    parentIn = nullptr;
                }
                if (parentOut) {
                    CloseHandle(parentOut);
                    parentOut = nullptr;
                }
                if (childIn) {
                    CloseHandle(childIn);
                    childIn = nullptr;
                }
                if (childOut) {
                    CloseHandle(childOut);
                    childOut = nullptr;
                }
                if (childProcess) {
                    CloseHandle(childProcess);
                    childProcess = nullptr;
                }
                if (childThread) {
                    CloseHandle(childThread);
                    childThread = nullptr;
                }
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
