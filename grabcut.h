/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#ifndef __GRABCUT_H__
#define __GRABCUT_H__

//#include "opencv2/imgcodecs.hpp"
#include "opencv2/highgui/highgui.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include <iostream>
void help();
const cv::Scalar RED = cv::Scalar(0,0,255);
const cv::Scalar PINK = cv::Scalar(230,130,255);
const cv::Scalar BLUE = cv::Scalar(255,0,0);
const cv::Scalar LIGHTBLUE = cv::Scalar(255,255,160);
const cv::Scalar GREEN = cv::Scalar(0,255,0);
const int BGD_KEY = cv::EVENT_FLAG_CTRLKEY;
const int FGD_KEY = cv::EVENT_FLAG_SHIFTKEY;
static void getBinMask( const cv::Mat& comMask, cv::Mat& binMask );
class GCApplication
{
    public:
        enum{ NOT_SET = 0, IN_PROCESS = 1, SET = 2 };
        static const int radius = 2;
        static const int thickness = -1;
        void reset();
        void setImageAndWinName( const cv::Mat& _image, const std::string& _winName );
        void showImage() const;
        void mouseClick( int event, int x, int y, int flags, void* param );
        int nextIter();
        int getIterCount() const { return iterCount; }
        void getMask(cv::Mat& rmask) const;
    private:
        void setRectInMask();
        void setLblsInMask( int flags, cv::Point p, bool isPr );
        const std::string* winName;
        const cv::Mat* image;
        cv::Mat mask;
        cv::Mat bgdModel, fgdModel;
        uchar rectState, lblsState, prLblsState;
        bool isInitialized;
        cv::Rect rect;
        std::vector<cv::Point> fgdPxls, bgdPxls, prFgdPxls, prBgdPxls;
        int iterCount;
};

#endif /* __GRABCUT_H__ */
