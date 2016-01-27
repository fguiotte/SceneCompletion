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

    normalize(_energy, _energy, 0, 1, NORM_MINMAX, CV_64FC3);

    run();
}

MinSeam::~MinSeam() {
}

void MinSeam::run() {
    cout << "Compute cumulative energy maps" << endl;
    computeEnergyCumMaps();
    cout << "Compute seams" << endl;
    computeMinimalSeams();
    cout << "Buils masks" << endl;
    buildMasks();
    cout << "Merge layers" << endl;
    mergeLayers();
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
        Point pointMinimum;
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                if (!i && !j) continue;
                Point potentialyNext(currentPoint.x + i, currentPoint.y + j);
                if (! bounds.contains(potentialyNext)) continue;
                if ((int)_mask.at<uchar>(potentialyNext) != 170) continue;
                if (energyMap.at<double>(potentialyNext) == -1.0) continue;
                if (energyMap.at<double>(potentialyNext) > valMin) continue;
                valMin = energyMap.at<double>(potentialyNext);
                pointMinimum = potentialyNext;
           }
           mySeam.push_back(pointMinimum);
           currentPoint = pointMinimum;
    }
    mySeam.push_back(_energyCum[index].second);
    _les_sims.push_back(mySeam);
}

unsigned int MinSeam::getMinSeamIndex() const {
    double energyMin = numeric_limits<double>::max();
    unsigned int energyMinIndex;

    for (vector<pair<Mat, Point> >::const_iterator it = _energyCum.begin(); it != _energyCum.end(); it++) {
        Point endSeam(it->second.x + 1, it->second.y);
        if (it->first.at<double>(endSeam) < energyMin) {
            energyMin = it->first.at<double>(endSeam);
            energyMinIndex = it - _energyCum.begin();
        }
    }

    return energyMinIndex;
}

cv::Mat MinSeam::showSeams() const {
    Mat bg, rgb;

    normalize(_energy, bg, 0, 255, NORM_MINMAX, CV_8UC1);

    vector<Mat> channels;
    channels.push_back(bg);
    channels.push_back(bg);
    channels.push_back(bg);
    merge(channels, rgb);

    vector<Point> minSeam = _les_sims[0];

    for (vector<vector<Point> >::const_iterator itseam = _les_sims.begin(); itseam != _les_sims.end(); itseam++)
        for (vector<Point>::const_iterator it = itseam->begin(); it != itseam->end(); it++)
            rgb.at<Vec3b>(*it) = Vec3b(0, 255, 0); 

    return rgb;
}

cv::Mat MinSeam::showSeam(unsigned int index) const {
    Mat ecum = getEnergyCum(index);
    Mat rgb;

    Mat ecumDisp;
    normalize(ecum, ecumDisp, 0, 255, NORM_MINMAX, CV_8UC1);

    vector<Mat> channels;
    channels.push_back(ecumDisp);
    channels.push_back(ecumDisp);
    channels.push_back(ecumDisp);
    merge(channels, rgb);

    vector<Point> minSeam = _les_sims[index];

    for (vector<Point>::const_iterator it = minSeam.begin(); it != minSeam.end(); it++) {
        rgb.at<Vec3b>(*it)[2] = 255;
        rgb.at<Vec3b>(*it)[1] = 0;
        rgb.at<Vec3b>(*it)[0] = 0;
    }

    return rgb;
}

void MinSeam::computeEnergyCumMaps() {
    //Point steam0(895, 708); // premier pixel du segment bord du patch ->  couture, que l'on appelle steam (Start Sim) 
    //Point steam1(895, 740); // dernier pixel 
    Point steam0, steam1;
    getSteamPoints(steam0, steam1);

    cout << "There is " << steam1.y - steam0.y << " energy maps to compute: "<< endl;
    for (int i = steam0.y; i < steam1.y; i++) { //  Pour chaque pixel de steam
        Point steamStart(steam0.x, i);
        cout << "Map [" << i - steam0.y + 1 << "/" << steam1.y - steam0.y << "]" << endl;
        neighbouroude(steam0, steam1, steamStart); // Init de eCum dans neighbouroude; duplique le for -1 dedans;
    }
}

void MinSeam::computeMinimalSeams() {
    for (unsigned int i = 0; i < _energyCum.size(); i++) {
        cout << "Seam [" << i + 1 << "/" << _energyCum.size() << "]" << endl;
        computeMinimalSeam(i);
    }
}
   
void MinSeam::neighbouroude (const Point & steam0, const Point & steam1, const Point & steamStart) { 
    // fonction récursive qui parcours le voisinage de chaque pixel et calcul l'énergie cumulée, stockée dans eCum   
    Mat eCum;
    initSteam(eCum, steam0, steam1);

    deque<Point> pointStack;

    Point firstPoint(steamStart.x - 1, steamStart.y); // We start at the left of the steam
    eCum.at<double>(firstPoint) = _energy.at<double>(firstPoint);
    pointStack.push_back(firstPoint);

    cv::Rect bounds(cv::Point(), eCum.size());

    while (! pointStack.empty()) {
        Point currentPoint = pointStack.front();
        pointStack.pop_front();
        double currentEnergy = eCum.at<double>(currentPoint);
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                // test si actuel
                if (!i && !j) continue;
                Point neighbour(currentPoint.x + i, currentPoint.y + j);
                // condition au bord
                if (! bounds.contains(currentPoint)) continue;
                // test si dans Probably Background
                if ((int)_mask.at<uchar>(neighbour) != 170) continue;
                // test si déjà visité ou si sur steam
                if (eCum.at<double>(neighbour) != 0) continue;
                eCum.at<double>(neighbour) = currentEnergy + _energy.at<double>(neighbour);
                smartEnergyCum(pointStack, eCum, neighbour);
            }
    }

    _energyCum.push_back(pair<Mat, Point>(eCum, steamStart));
}

void MinSeam::initSteam(Mat & energy, const Point & steamBegin, const Point & steamEnd) const {
    energy.create(_energy.size(), CV_64FC1);
    energy = Scalar(0);
    for (int i = steamBegin.y ; i < steamEnd.y ; i++)
        energy.at<double>(Point(steamBegin.x,i)) = -1;
}

void MinSeam::smartEnergyCum(deque<Point> & stack, const Mat & values, const Point & point) const {
    for (deque<Point>::iterator it = stack.begin(); it != stack.end(); it++)
        if (values.at<double>(point) < values.at<double>(*it)) {
            stack.insert(it, point);
            return;
        }
    stack.push_back(point);
}

Mat MinSeam::getBinMask() const {
    vector<Point> minSeam = _les_sims[getMinSeamIndex()];
    Mat binMask(_mask.size(), CV_8UC1);
    binMask = Scalar(0);

    queue<Point> foregroundStack;
    foregroundStack.push(getAMaskPoint());
    binMask.at<uchar>(foregroundStack.front()) = 1;

    //int qwe;

    while (! foregroundStack.empty()) {
        Point currentPoint = foregroundStack.front();
        foregroundStack.pop();
        for (int i = -1; i <= 1; i++)
            for (int j = -1; j <= 1; j++) {
                if (abs(i) == abs(j)) continue;
                Point neighbour(currentPoint.x + i, currentPoint.y + j);
                if (binMask.at<uchar>(neighbour) == 1) continue;
                if (_mask.at<uchar>(neighbour) == 255) {
                    binMask.at<uchar>(neighbour) = 1;
                    foregroundStack.push(neighbour);
                    continue;
                }
                if (find(minSeam.begin(), minSeam.end(), neighbour) != minSeam.end()) continue; // TODO: Hurr durr
                binMask.at<uchar>(neighbour) = 1;
                foregroundStack.push(neighbour);
            }
        //qwe++;
        //if (qwe%100 == 0) {
        //    imshow("tmp", norm_0_255(binMask));
        //    waitKey(1);
        //}
    }

    return binMask;
}


Point MinSeam::getAMaskPoint() const {
    for (int i = 0; i < _mask.cols; i++)
        for (int j = 0; j < _mask.rows; j++)
            if (_mask.at<uchar>(Point(i, j)) == 255) return Point(i, j);
    return Point();
}

void MinSeam::buildMasks() {
    getBinMask().copyTo(_fgMask);
    _bgMask = _fgMask == 0;
}

void MinSeam::mergeLayers() {
    _foreground.copyTo(_result, _fgMask);
    _background.copyTo(_result, _bgMask);
}

Mat MinSeam::getResult() const {
    return _result;
}

void MinSeam::getSteamPoints(Point & steamBegin, Point & steamEnd) const {
    Point currentSteamBegin, currentSteamEnd;
    int bestSteam = numeric_limits<int>::max();
    for (int c = 0; c < _mask.cols; c++) {
        bool fgpresent = false;
        int pbgheight = 0;
        for (int r = 0; r < _mask.rows; r++) {
            if (_mask.at<uchar>(Point(c, r)) == 170) {
                if (pbgheight == 0) currentSteamBegin = Point(c, r);
                pbgheight++;
                continue;
            }
            if (_mask.at<uchar>(Point(c, r)) == 255) {
                currentSteamEnd = Point(c, r);
                fgpresent = true;
                //r = _mask.rows;
                break;
            }
        }
        if (fgpresent) {
            if (pbgheight < bestSteam) {
                cout << "Col " << c << " Best : " << pbgheight << endl;
                steamBegin = currentSteamBegin;
                steamEnd = currentSteamEnd;
                bestSteam = pbgheight;
            }
        }
    }
}
