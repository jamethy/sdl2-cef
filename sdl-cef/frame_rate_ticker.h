//
// Created by james on 7/29/18.
//

#ifndef FRAME_RATE_TICKER_H
#define FRAME_RATE_TICKER_H

#include <chrono>

/**
 * Class unrelated to Cef or SDL just used to manage the frame rate
 */
class FrameRateTicker {
public:

    explicit FrameRateTicker(double fps);

    void tick();

private:

    std::chrono::steady_clock::time_point frameStart;
    long millisecondsPerFrame;
};

#endif // FRAME_RATE_TICKER_H
