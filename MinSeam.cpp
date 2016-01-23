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
#include <limits>
#include "utils.h"
using namespace cv;
using namespace std;

MinSeam::MinSeam(const cv::Mat & background, const cv::Mat & foreground, const cv::Mat & mask) :
    _background(background),
    _foreground(foreground),
    _mask(mask) 
{
    Mat_<Vec3d> bgf, fgf;
    background.convertTo(bgf, CV_64FC3);
    foreground.convertTo(fgf, CV_64FC3);

    Mat energy;
    pow((bgf - fgf), 2, energy);
    vector<Mat> channels;
    split(energy, channels);
    pow((channels[0] + channels[1] + channels[2]), 0.5, _energy);
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
    return _energyCum[i].first;
}

void MinSeam::computeMinimalSeam(unsigned int index) { //compute One Seam for One Energy Map
    vector<Point> mySeam;
    Mat energyMap = _energyCum[index].first;
    Point firstPoint = Point(_energyCum[index].second.x - 1, _energyCum[index].second.y);// LOL
    Point lastPoint = Point(_energyCum[index].second.x + 1, _energyCum[index].second.y); // LOL 
    Point currentPoint = lastPoint;
    cv::Rect bounds(cv::Point(), energyMap.size());
    mySeam.push_back(lastPoint);
    while(currentPoint != firstPoint){
        double valMin = energyMap.at<double>(currentPoint);
        cout << currentPoint << ":"  << valMin << endl;

        Point pointMinimum;
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                Point potentialyNext(currentPoint.x + i, currentPoint.y + j);
                // condition au bord
                if (! bounds.contains(potentialyNext)) continue;
                // test si actuel
                if (!i && !j) continue;
                // test si dans Probably Background
                if ((int)_mask.at<uchar>(potentialyNext) != 170) continue;
                if (energyMap.at<double>(potentialyNext) == -1.0) continue;
                if (energyMap.at<double>(potentialyNext) <= valMin)
                {
                    valMin = energyMap.at<double>(potentialyNext);
                    pointMinimum = potentialyNext;
                }

           }
           mySeam.push_back(pointMinimum);
           currentPoint = pointMinimum;
    }
    mySeam.push_back(_energyCum[index].second);
    _les_sims.push_back(mySeam);
}

cv::Mat MinSeam::showMinimalSeam(unsigned int index) {
    computeMinimalSeam(index);
    Mat ecum = getEnergyCum(index);
    Mat rgb;

    Mat ecumDisp;
    normalize(ecum, ecumDisp, 0, 255, NORM_MINMAX, CV_8UC1);

    vector<Mat> channels;
    channels.push_back(ecumDisp);
    channels.push_back(ecumDisp);
    channels.push_back(ecumDisp);
    merge(channels, rgb);

    vector<Point> minSeam = _les_sims[index - index];

    for (vector<Point>::const_iterator it = minSeam.begin(); it != minSeam.end(); it++) {
        rgb.at<Vec3b>(*it)[2] = 255;
        rgb.at<Vec3b>(*it)[1] = 0;
        rgb.at<Vec3b>(*it)[0] = 0;
        cout << *it << endl;
    }

    return rgb;
}

void MinSeam::computeEnergyCumMaps() {
    Point steam0(895, 708); // premier pixel du segment bord du patch ->  couture, que l'on appelle steam (Start Sim) 
    Point steam1(895, 740); // dernier pixel 

   for (int i = steam0.y; i < steam1.y; i++) { //  Pour chaque pixel de steam
        Point steamStart(steam0.x, i);
        neighbouroude(steam0, steam1, steamStart); // Init de eCum dans neighbouroude; duplique le for -1 dedans;
    }
}
   
void MinSeam::neighbouroude (const Point & steam0, const Point & steam1, const Point & steamStart) { 
    // fonction récursive qui parcours le voisinage de chaque pixel et calcul l'énergie cumulée, stockée dans eCum   
    Mat_<double> eCum(_energy.rows, _energy.cols, CV_64FC1);
    eCum.setTo(0);
    for (int i = steam0.y ; i < steam1.y ; i++)
        eCum.at<double>(Point(steam0.x,i)) = -1;


    Point firstPoint(steamStart.x - 1, steamStart.y); // We start at the left of the steam

    queue<Point> pointStack;
    eCum.at<double>(firstPoint) = _energy.at<float>(firstPoint);

    cout << "Energie de départ : " <<  eCum.at<double>(firstPoint) << endl;

    pointStack.push(firstPoint);

    cv::Rect bounds(cv::Point(), eCum.size());

   // char window_name[] = "MinSeam demo";
   // namedWindow(window_name, WINDOW_AUTOSIZE);
    while (! pointStack.empty()) {
    Point currentPoint = pointStack.front();
    pointStack.pop();

    //imshow(window_name, norm_0_255(eCum));
   // waitKey(1);
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
                // test si déjà visité ou si sur steam
                if (eCum.at<double>(neighbour) != 0) continue; // Probably okay...
                if (eCum.at<double>(neighbour) == -1.0) continue;
                // test si dans Probably Background
                if ((int)_mask.at<uchar>(neighbour) != 170)
                    continue;
                eCum.at<double>(neighbour) = currentEnergy + _energy.at<double>(neighbour);
                pointStack.push(neighbour);
            }
    }

    _energyCum.push_back(pair<Mat, Point>(eCum, steamStart));

//         
//
//    // TODO : pop le candidat traité
//    eCum = neighbouroude(steam0, steam1, v);
//    if (v == steamStart) return eCum;
}
