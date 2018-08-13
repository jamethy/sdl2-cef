#include "frame_rate_ticker.h"

#include <thread>

using std::chrono::steady_clock;
using std::chrono::milliseconds;
using std::chrono::duration_cast;

FrameRateTicker::FrameRateTicker(double fps) {
    millisecondsPerFrame = static_cast<long>(1000 / fps);

    // note the start time of the frame
    frameStart = steady_clock::now();
}

void FrameRateTicker::tick() {
    auto elapsed = duration_cast<milliseconds>(steady_clock::now() - frameStart).count();
    if (elapsed < millisecondsPerFrame) {
        std::this_thread::sleep_for(milliseconds(millisecondsPerFrame - elapsed));
    }
    frameStart = steady_clock::now();
}
