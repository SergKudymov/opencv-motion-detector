#include <iostream>
#include <string>
#include <opencv2/opencv.hpp>

using namespace std;

int digKey = 0;
int regionCoordinates[10][4];

void myMouseCallback(int event, int x, int y, int flags, void* frame)
{
    switch (event) {
    case cv::EVENT_MOUSEMOVE:
        //printf("%d x %d\n", x, y);
        break;

    case cv::EVENT_LBUTTONDOWN:
        //printf("%d x %d\n", region_coordinates[dig_key][0], region_coordinates[dig_key][1]);
        if (regionCoordinates[digKey][0] != 0 && regionCoordinates[digKey][1] != 0 && regionCoordinates[digKey][2] == 0 && regionCoordinates[digKey][3] == 0)
        {
            regionCoordinates[digKey][2]=x;
            regionCoordinates[digKey][3]=y;
        }
        if (regionCoordinates[digKey][0] == 0 && regionCoordinates[digKey][1] == 0)
        {
            regionCoordinates[digKey][0]=x;
            regionCoordinates[digKey][1]=y;
        }
        break;

    case cv::EVENT_FLAG_RBUTTON:
        break;
    case cv::EVENT_LBUTTONUP:
        break;
    }
}


int main()
{
    std::time_t timeBegin = std::time(0);
    int tick = 0;
    int fps = 0;
    std::string textFps = "No fps";

    cv::Mat frame;
    cv::VideoCapture cap;
    cap.open(-1); // load default camera

    if (!cap.isOpened()) {
        cerr << "ERROR! Unable to open video\n";
        return -1;
    }
    while(1)
    {
        // wait for a new frame from camera and store it into 'frame'
        cap.read(frame);
        // check if we succeeded
        if (frame.empty()) {
            break;
        }

        //---------------------------------------------------------------------------
        // Display fps

        std::time_t timeNow = std::time(0) - timeBegin;
        if (tick == timeNow)
        {
            fps++;
        }
        else
        {
            tick=timeNow;
            textFps = std::to_string(fps);
            fps=0;
        }
        cv::putText(frame,
                    "FPS: " + textFps,
                    cv::Point(5, 20),
                    cv::HersheyFonts::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar::all(255));

        //---------------------------------------------------------------------------

        cv::setMouseCallback("Motion", myMouseCallback);
        if (regionCoordinates[digKey][0] != 0 && regionCoordinates[digKey][1] != 0 && regionCoordinates[digKey][2] == 0 && regionCoordinates[digKey][3] == 0)
            cv::rectangle(frame,
                          cv::Point(regionCoordinates[digKey][0], regionCoordinates[digKey][1]),
                    cv::Point(regionCoordinates[digKey][0]+1, regionCoordinates[digKey][1]+1),
                    cv::Scalar(0,0,255),
                    2,
                    cv::LINE_AA,
                    0);

        if (regionCoordinates[digKey][0] != 0 && regionCoordinates[digKey][1] != 0 && regionCoordinates[digKey][2] != 0 && regionCoordinates[digKey][3] != 0)
            cv::rectangle(frame,
                          cv::Point(regionCoordinates[digKey][0], regionCoordinates[digKey][1]),
                    cv::Point(regionCoordinates[digKey][2],regionCoordinates[digKey][3]),
                    cv::Scalar(0,0,255),
                    2,
                    cv::LINE_AA,
                    0);


        char c = cv::waitKey(20);
        if (c <= 57 && c >= 48)
        {
            digKey = c-48; // key "0123456789"
        }

        //---------------------------------------------------------------------------

        // show live and wait for a key with timeout long enough to show images
        cv::imshow("Motion", frame);
        char qButton = cv::waitKey(10);
        if (qButton == 113)
            break;
    }
    return 0;
}
