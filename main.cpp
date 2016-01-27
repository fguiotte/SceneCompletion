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

void demo1();
void demo2();
void demo3();

static void on_mouse(int event, int x, int y, int flags, void* param )
{
    gcapp.mouseClick( event, x, y, flags, param );
}

int main( int argc, char* argv[] ) {
    demo1();
    demo2();
    demo3();

    return 0;
}

void demo1() {
    Mat bg, fg, mask;
    bg = imread("../img/demo/bg.jpg");
    fg = imread("../img/demo/fg.jpg");
    mask = imread("../img/demo/mask.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, MS_BASICLAB_E);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());

    imshow("Tadaa", ms.getResult());
    waitKey(0);
}

void demo2() {
    Mat bg, fg, mask;
    bg = imread("../img/demo2/cible2.jpg");
    fg = imread("../img/demo2/source2.jpg");
    mask = imread("../img/demo2/mask2b.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, MS_BASICLAB_E);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());

    imshow("Tadaa", ms.getResult());
    waitKey(0);
}

void demo3() {
    Mat bg, fg, mask;
    bg = imread("../img/demo2/cible.jpg");
    fg = imread("../img/demo2/source.jpg");
    mask = imread("../img/demo2/mask.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, MS_BASICLAB_E);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());

    imshow("Tadaa", ms.getResult());
    waitKey(0);
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
