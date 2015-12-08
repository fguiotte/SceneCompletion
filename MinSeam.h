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

class MinSeam {
    public:
        MinSeam(const cv::Mat & background, const cv::Mat & foreground, const cv::Mat & mask);
        virtual ~MinSeam();
        cv::Mat getEnergy() const;
        void computeEnergyCumMaps();

    private:
        cv::Mat _background;
        cv::Mat _foreground;
        cv::Mat _mask;

    private:
        cv::Mat _energy;
        std::vector<cv::Mat> _energyCum;
};

#endif /* __MINSEAM_H__ */
