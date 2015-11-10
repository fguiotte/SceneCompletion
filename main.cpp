/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include <iostream>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

int main( int argc, char* argv[] )
{
    cv::Mat src;
    cv::Mat dst;
    char window_name[] = "Demo 1";

    src = cv::imread("../img/lena.jpg", 1);
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    imshow(window_name, src);
    cv::waitKey(0);

    //cv::grabCut();

    return 0;
}


