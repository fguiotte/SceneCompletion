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
#include "grabcut.h"
#include "MinSeam.h"
#include "utils.h"

using namespace std;
using namespace cv;

GCApplication gcapp;

static void on_mouse(int event, int x, int y, int flags, void* param )
{
    gcapp.mouseClick( event, x, y, flags, param );
}

int main( int argc, char* argv[] ) {
    Mat bg, fg, mask;
    bg = imread("../img/demo/bg.jpg");
    fg = imread("../img/demo/fg.jpg");
    mask = imread("../img/demo/mask.png");

    MinSeam ms(bg, fg, mask);
    Mat energy = ms.getEnergy();

    char window_name[] = "MinSeam demo";
    namedWindow(window_name, WINDOW_AUTOSIZE);
    imshow(window_name, norm_0_255(energy));

    ms.computeEnergyCumMaps();
    Mat energyCum = ms.getEnergyCum(10);
    char window2_name[] = "Energy cum demo";
    namedWindow(window2_name, WINDOW_AUTOSIZE);
    Mat energyCumLog;
    log(1 + energyCum, energyCumLog);
    imshow(window2_name, norm_0_255(energyCumLog));

    int quit = 0;
    while (quit != 113)
        quit = waitKey();

    cout << "Done." << endl;
    return 0;
}

int old_main( int argc, char* argv[] )
{
    cv::Mat src;
    cv::Mat dst;
    char window_name[] = "Demo 1";

    cout << "Open image" << endl;
    src = cv::imread("../img/lena.jpg", 1);
    cv::namedWindow(window_name, cv::WINDOW_AUTOSIZE);
    imshow(window_name, src);
    cv::waitKey(0);

    // Filter for better segmentation
    cout << "Filter image" << endl;
    Mat im2, im1;
    src.copyTo(im1);
    for (int i = 0; i < 6; i++) {
        bilateralFilter(im1, im2, 10, 20.0, 20.0);
        im2.copyTo(im1);
    }
    imshow(window_name, im1);

    // Select object using grabcut
    cout << "Select object" << endl;
    const string winName = "Select object";
    namedWindow( winName, WINDOW_AUTOSIZE );
    setMouseCallback( winName, on_mouse, 0 );
    gcapp.setImageAndWinName( im1, winName );
    gcapp.showImage();

    while (true) {
        int c = waitKey(0);
        if (c == 27) break; // Escape
        if (c == 114) {     // r
            cout << "Reset" << endl;
            gcapp.reset();
            gcapp.showImage();
        }
        if (c == 10) {      // Enter
            for (int i = 0; i < 10; i++) {
                cout << "Iteration " << i << endl;
                gcapp.nextIter();
            }
            gcapp.showImage();
            break;
        }

    }

    gcapp.getMask(im1);
    im1 *= 85;
    imshow(window_name, im1);
    waitKey(0);

    cout << im1;
    cout << "Quit" << endl;

    return 0;
}


