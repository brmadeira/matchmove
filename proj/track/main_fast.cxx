#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>
#include <opencv2/imgproc.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/video.hpp>

using namespace cv;
using namespace std;

#define SQR(X) ((X)*(X))

static double min_distancy(Point2f p2, const vector<Point2f>& p1)
{
    double min_dist = DBL_MAX;

    for (const auto& p : p1)
    {
        double dist = SQR(p2.x - p.x) + SQR(p2.y - p.y);
        if (dist < min_dist)
            min_dist = dist;
    }
    return min_dist;
}

int main(int argc, char **argv)
{
    VideoCapture capture(argv[1]);

    Mat old_frame, old_gray;
    vector<Point2f> p0, p1;

    int npoints = atoi(argv[2]);
    double features_dist = atof(argv[3]);

    capture >> old_frame;
    if (old_frame.empty())
        return 0;

    cvtColor(old_frame, old_gray, COLOR_BGR2GRAY);
    goodFeaturesToTrack(old_gray, p0, npoints, 0.01, features_dist);

    cout << "Features per frame = " << argv[2] << endl;
    cout << "Frame 0" << endl;

    for (size_t i = 0; i < p0.size(); i++)
        cout << p0[i].x << " " << p0[i].y << " 0" << endl;

    int k = 1;

    while (true)
    {
        Mat frame, frame_gray;

        capture >> frame;
        if (frame.empty())
            break;

        cout << "Frame " << k++ << endl;

        cvtColor(frame, frame_gray, COLOR_BGR2GRAY);

        vector<uchar> status;
        vector<float> err;

        calcOpticalFlowPyrLK(old_gray, frame_gray, p0, p1, status, err);

        vector<Point2f> good_new;

        for (size_t i = 0; i < p0.size(); i++)
        {
            if (status[i])
            {
                good_new.push_back(p1[i]);
                circle(frame, p1[i], 2, Scalar(0, 0, 255), -1);
                cout << p1[i].x << " " << p1[i].y << " 0" << endl;
            }
            else
            {
                cout << p0[i].x << " " << p0[i].y << " -1" << endl;

                vector<Point2f> new_pts;
                goodFeaturesToTrack(old_gray, new_pts, 1, 0.01, features_dist);

                if (!new_pts.empty())
                {
                    Point2f candidate = new_pts[0];

                    if (min_distancy(candidate, good_new) > features_dist)
                        good_new.push_back(candidate);
                }
            }
        }

        imshow("Frame", frame);

        int keyboard = waitKey(30);
        if (keyboard == 'q' || keyboard == 27)
            break;

        old_gray = frame_gray.clone();
        p0 = good_new;
    }

    return 0;
}
