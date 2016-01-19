/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#include "MinSeam.h"
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include <queue>
#include "utils.h"
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

Mat MinSeam::getEnergyCum(unsigned int i) const {
    if ( i > _energyCum.size()) {
        cout << "Wrong energy cumul index" << endl;
        exit(2);
    }
    return _energyCum[i];
}

void MinSeam::computeEnergyCumMaps() {
    Point steam0(895, 708); // premier pixel du segment bord du patch ->  couture, que l'on appelle steam (Start Sim) 
    Point steam1(895, 739); // dernier pixel 

   for (int i = steam0.y; i < steam1.y; i++) { //  Pour chaque pixel de steam
        Point steamStart(steam0.x, i);
        neighbouroude(steam0, steam1, steamStart); // Init de eCum dans neighbouroude; duplique le for -1 dedans;
    }
}
   
void MinSeam::neighbouroude (Point steam0, Point steam1, Point steamStart) { 
    // fonction récursive qui parcours le voisinage de chaque pixel et calcul l'énergie cumulée, stockée dans eCum   
    Mat_<double> eCum(_energy.rows, _energy.cols, CV_64FC1);
    eCum.setTo(0);
    for (int i = steam0.y ; i < steam1.y ; i++)
        eCum.at<double>(steam0.x,i) = -1.0f;

    Point firstPoint(steamStart.x - 1, steamStart.y); // We start at the left of the steam

    queue<Point> pointStack;
    eCum.at<double>(firstPoint) = _energy.at<float>(firstPoint);

    cout << "Energie de départ : " <<  eCum.at<double>(firstPoint) << endl;

    pointStack.push(firstPoint);

    cv::Rect bounds(cv::Point(), eCum.size());

//    char window_name[] = "MinSeam demo";
//    namedWindow(window_name, WINDOW_AUTOSIZE);
    while (! pointStack.empty()) {
    Point currentPoint = pointStack.front();
    pointStack.pop();

//    imshow(window_name, norm_0_255(eCum));
//    waitKey(1);
//
//    // TODO: prendre en compte les filtres

        double currentEnergy = eCum.at<double>(currentPoint);
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                // test si actuel
                if (!i && !j) continue;
                Point neighbour(currentPoint.x + i, currentPoint.y + j);
                // condition au bord
                if (! bounds.contains(currentPoint)) continue;
                // test si déjà visité
                if (eCum.at<double>(neighbour) != 0) continue; // Probably okay...
                eCum.at<double>(neighbour) = currentEnergy + _energy.at<double>(neighbour);
                pointStack.push(neighbour);
            }
    }

    _energyCum.push_back(eCum);

//         
//
//    // TODO : pop le candidat traité
//    eCum = neighbouroude(steam0, steam1, v);
//    if (v == steamStart) return eCum;
}
