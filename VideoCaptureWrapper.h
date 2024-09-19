#pragma once

#include <opencv2/opencv.hpp>
#include <memory>

using namespace cv;

class VideoCaptureWrapper
{
public:
    VideoCaptureWrapper() : cap(std::make_unique<VideoCapture>()) {}

    bool open(int device)
    {
        return cap->open(device);
    }

    bool isOpened() const
    {
        return cap->isOpened();
    }

    bool read(Mat& frame)
    {
        return cap->read(frame);
    }

    void release()
    {
        if (cap->isOpened())
        {
            cap->release();
        }
    }

    void clear()
    {
        cap.reset();
    }

    VideoCapture& getCapture()
    {
        return *cap;
    }

private:
    std::unique_ptr<VideoCapture> cap;
};