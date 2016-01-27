/***************************************************************************
 *
 * TODO: Project Title
 *
 * Author: Florent Guiotte <guiotte.florent@gmail.com>
 *
 **************************************************************************/

#ifndef __MINSEAM_H__
#define __MINSEAM_H__
#include <opencv2/core/core.hpp>
#include <vector>
#include <deque>

class MinSeam {
    public:
        MinSeam(const cv::Mat & background, const cv::Mat & foreground, const cv::Mat & mask);
        virtual ~MinSeam();
        cv::Mat getEnergy() const;
        cv::Mat getEnergyCum(unsigned int i = 0) const;
        cv::Mat showSeam(unsigned int index = 0) const;
        cv::Mat showSeams() const;
        cv::Mat getResult() const;

    private:
        cv::Mat _background;
        cv::Mat _foreground;
        cv::Mat _mask;
        cv::Mat _fgMask;
        cv::Mat _bgMask;
        std::vector<std::vector<cv::Point> > _les_sims;
        cv::Mat _result;

    private:
        cv::Mat_<double> _energy;
        std::vector<std::pair<cv::Mat, cv::Point > > _energyCum;

    private:
        void run();
        void neighbouroude (const cv::Point & steam0, const cv::Point & steam1, const cv::Point & steamStart);
        void smartEnergyCum(std::deque<cv::Point> & stack, const cv::Mat & values, const cv::Point & point) const;
        void initSteam(cv::Mat & energy, const cv::Point & steamBegin, const cv::Point & steamEnd) const;
        void computeEnergyCumMaps();
        void computeMinimalSeam(unsigned int index);
        void computeMinimalSeams();
        cv::Point getAMaskPoint() const;
        void buildMasks();
        void mergeLayers();
        unsigned int getMinSeamIndex() const;
        cv::Mat getBinMask() const;
        void getSteamPoints(cv::Point & steamBegin, cv::Point & steamEnd) const;
};

#endif /* __MINSEAM_H__ */
