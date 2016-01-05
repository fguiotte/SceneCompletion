/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include "utils.h"
using namespace cv;

Mat norm_0_255(InputArray _src) {
 Mat src = _src.getMat();
 // Create and return normalized image:
 Mat dst;
 switch(src.channels()) {
    case 1:
        normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC1);
        break;
    case 3:
        normalize(_src, dst, 0, 255, NORM_MINMAX, CV_8UC3);
        break;
    default:
    src.copyTo(dst);
    break;
 }
 return dst;
}
