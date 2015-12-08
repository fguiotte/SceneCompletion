/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include "MinSeam.h"
using namespace cv;

MinSeam::MinSeam(const cv::Mat & background, const cv::Mat & foreground, const cv::Mat & mask) :
    _background(background),
    _foreground(foreground),
    _mask(mask) {
    pow((background - foreground), 2, _energy);
    vector<Mat> channels;
    split(_energy, channels);
    _energy = channels[0] + channels[1] + channels[2];
    //cout << _energy << endl;
}

MinSeam::~MinSeam() {
}

Mat MinSeam::getEnergy() const {
    return _energy;
}

void MinSeam::computeEnergyCumMaps() {
}
