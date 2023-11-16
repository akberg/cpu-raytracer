#include <chrono>
#include <iostream>

class TaskTimer {
public:
    void start(std::string msg) {
        tStart = std::chrono::system_clock::now();
        std::cerr << msg;
    }
    void stop() {
        auto elapsed = std::chrono::duration_cast<std::chrono::milliseconds>(
            std::chrono::system_clock::now() - tStart);
        std::cerr << elapsed.count() << "ms\n";
    }
    void stopRestart(std::string msg) {
        stop();
        start(msg);
    }

private:
    std::chrono::_V2::system_clock::time_point tStart;
};
