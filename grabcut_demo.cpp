/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include "grabcut.h"
using namespace std;
using namespace cv;

GCApplication gcapp;

static void on_mouse(int event, int x, int y, int flags, void* param )
{
    gcapp.mouseClick( event, x, y, flags, param );
}

int main( int argc, char** argv )
{
    if( argc!=2 )
    {
        help();
        return 1;
    }
    string filename = argv[1];
    if( filename.empty() )
    {
        cout << "\nDurn, couldn't read in " << argv[1] << endl;
        return 1;
    }
    Mat image = imread( filename, 1 );
    if( image.empty() )
    {
        cout << "\n Durn, couldn't read image filename " << filename << endl;
        return 1;
    }
    Mat im2;
    for (int i = 0; i < 6; i++) {
        bilateralFilter(image, im2, 10, 20.0, 20.0);
        im2.copyTo(image);
    }
    help();
    const string winName = "image";
    namedWindow( winName, WINDOW_AUTOSIZE );
    setMouseCallback( winName, on_mouse, 0 );
    gcapp.setImageAndWinName( image, winName );
    gcapp.showImage();
    for(;;)
    {
        int c = waitKey(0);
        switch( (char) c )
        {
            case '\x1b':
                cout << "Exiting ..." << endl;
                goto exit_main;
            case 'r':
                cout << endl;
                gcapp.reset();
                gcapp.showImage();
                break;
            case 'n':
                int iterCount = gcapp.getIterCount();
                cout << "<" << iterCount << "... ";
                int newIterCount = gcapp.nextIter();
                if( newIterCount > iterCount )
                {
                    gcapp.showImage();
                    cout << iterCount << ">" << endl;
                }
                else
                    cout << "rect must be determined>" << endl;
                break;
        }
    }
exit_main:
    destroyWindow( winName );
    return 0;
}
