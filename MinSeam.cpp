/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include "MinSeam.h"
#include <iostream>
#include <queue>
using namespace cv;
using namespace std;

MinSeam::MinSeam(const cv::Mat & background, const cv::Mat & foreground, const cv::Mat & mask) :
    _background(background),
    _foreground(foreground),
    _mask(mask) 
{
    Mat_<Vec3f> bgf, fgf;
    background.convertTo(bgf, CV_32FC3);
    foreground.convertTo(fgf, CV_32FC3);

    pow((bgf - fgf), 2, _energy);
    vector<Mat> channels;
    split(_energy, channels);
    _energy = channels[0] + channels[1] + channels[2];
}

MinSeam::~MinSeam() {
}

Mat MinSeam::getEnergy() const {
    return _energy;
}

void MinSeam::computeEnergyCumMaps() {
    Point steam0(895, 708); // premier pixel du segment bord du patch ->  couture, que l'on appelle steam (Start Sim) 
    Point steam1(895, 739); // dernier pixel 

   for (int i = steam0.y; i < steam1.y; i++) { //  Pour chaque pixel de steam
        Point steamStart(steam0.x, i);
        Mat eCum = neighbouroude(steam0, steam1, steamStart); // Init de eCum dans neighbouroude; duplique le for -1 dedans;
    }
}
   
Mat_<double> MinSeam::neighbouroude (Point steam0, Point steam1, Point steamStart) { 
    // fonction récursive qui parcours le voisinage de chaque pixel et calcul l'énergie cumulée, stockée dans eCum   
    Mat_<double> eCum(_energy.rows, _energy.cols, CV_32FC1);
    for (int i = steam0.y ; i < steam1.y ; i++)
        eCum.at<double>(steam0.x,i) = -1.0f;

    Point firstPoint(steamStart.x - 1, steamStart.y); // We start at the left of the steam

    queue<Point> pointStack;
    eCum.at<double>(firstPoint) = _energy.at<double>(firstPoint);

    pointStack.push(firstPoint);

    while (! pointStack.empty()) {
    Point currentPoint = pointStack.pop();

    double currentEnergy = eCum.at<double>(currentPoint);
    for (int i = -1; i <= 1; i++)
        for (int j = -1; j <= 1; j++) {
            // TODO: condition au bord
            // TODO: test si déjà visité
            if (!i && !j) continue;
            Point neighbour(currentPoint.x + i, currentPoint.y + j);
            pointStack.push(neighbour);
            eCum.at<double>(neighbour) = currentEnergy + _energy.at<double>(neighbour);
        }
    }

//         
//
//    // TODO : pop le candidat traité
//    eCum = neighbouroude(steam0, steam1, v);
//    if (v == steamStart) return eCum;
}
