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

void demo1(int etype = 0);
void demo2(int etype = 0);
void demo2_write(int etype = 0);
void demo3(int etype = 0);

static void on_mouse(int event, int x, int y, int flags, void* param )
{
    gcapp.mouseClick( event, x, y, flags, param );
}

int main( int argc, char* argv[] ) {
    int etype;
    if (argc == 2) {
        string a(argv[1]);
        if (a == "basic") etype = MS_BASIC_E;
        if (a == "sobel") etype = MS_SOBEL_E;
        if (a == "basiclab") etype = MS_BASICLAB_E;
    } else
        etype = 0;

    demo1(etype);
    demo2(etype);
    //demo2_write(etype);
    demo3(etype);

    return 0;
}

void demo1(int etype) {
    Mat bg, fg, mask;
    bg = imread("../img/bg1.jpg");
    fg = imread("../img/fg1.jpg");
    mask = imread("../img/mask1.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, etype);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());

    imshow("Tadaa", ms.getResult());
    waitKey(0);
}

void demo2(int etype) {
    Mat bg, fg, mask;
    bg = imread("../img/bg2.jpg");
    fg = imread("../img/fg2.jpg");
    mask = imread("../img/mask2.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, etype);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());

    imshow("Tadaa", ms.getResult());
    waitKey(0);
}

void demo2_write(int etype) {
    Mat bg, fg, mask;
    bg = imread("../img/bg2.jpg");
    fg = imread("../img/fg2.jpg");
    mask = imread("../img/mask2.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, etype);

    imshow("Energy", ms.getEnergy());
    imshow("Seam", ms.showSeams());
    imshow("Tadaa", ms.getResult());


    Mat e = ms.getEnergy();
    Mat eb;
    normalize(e, eb, 0, 255, NORM_MINMAX, CV_8UC3);

    unsigned int i = ms.getMinSeamIndex();

    Mat ec, dms;
    ec = ms.getEnergyCum(i);
    Mat ecb;
    normalize(ec, ecb, 0, 255, NORM_MINMAX, CV_8UC3);


    if (etype == 1) {
        imwrite("energy_sobel.png", eb);
        imwrite("min_seam_sobel.png", ms.showSeam(i));
        imwrite("all_seams_sobel.png", ms.showSeams());
        imwrite("energy_cum_sobel.png", ecb);
        imwrite("result_sobel.png", ms.getResult());
    } else {
        imwrite("energy.png", eb);
        imwrite("min_seam.png", ms.showSeam(i));
        imwrite("all_seams.png", ms.showSeams());
        imwrite("energy_cum.png", ecb);
        imwrite("result.png", ms.getResult());
    }
    
    waitKey(0);
}

void demo3(int etype) {
    Mat bg, fg, mask;
    bg = imread("../img/bg3.jpg");
    fg = imread("../img/fg3.jpg");
    mask = imread("../img/mask3.png", CV_8UC1);

    MinSeam ms(bg, fg, mask, etype);

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
