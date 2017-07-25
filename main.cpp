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
            regionCoordinates[digKey][2] = x;
            regionCoordinates[digKey][3] = y;
        }
        if (regionCoordinates[digKey][0] == 0 && regionCoordinates[digKey][1] == 0)
        {
            regionCoordinates[digKey][0] = x;
            regionCoordinates[digKey][1] = y;
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
    char charCheckForEscKey = 0;

    std::time_t timeBegin = std::time(0);
    int tick = 0;
    int fps = 0;
    std::string textFps = "No fps";

    cv::Mat imgOriginal;
    cv::Mat imgHSV;
    cv::Mat imgThreshLow;
    cv::Mat imgThreshHigh;
    cv::Mat imgThresh;

    std::vector<cv::Vec3f> v3fCircles;				// 3 element vector of floats, this will be the pass by reference output of HoughCircles()

    cv::VideoCapture capWebCam;
    capWebCam.open(-1); // load default camera

    if (!capWebCam.isOpened()) {
        cerr << "ERROR! Unable to open video\n";
        return -1;
    }
    while(charCheckForEscKey != 27 && capWebCam.isOpened())
    {
        // wait for a new frame from camera and store it into 'frame'
        capWebCam.read(imgOriginal);
        // check if we succeeded
        if (imgOriginal.empty()) {
            break;
        }

        //---------------------------------------------------------------------------
        //Motion detector

        cv::cvtColor(imgOriginal, imgHSV, CV_BGR2HSV);

        cv::inRange(imgHSV, cv::Scalar(0, 155, 155), cv::Scalar(18, 255, 255), imgThreshLow);
        cv::inRange(imgHSV, cv::Scalar(165, 155, 155), cv::Scalar(179, 255, 255), imgThreshHigh);

        cv::add(imgThreshLow, imgThreshHigh, imgThresh);

        cv::GaussianBlur(imgThresh, imgThresh, cv::Size(3, 3), 0);

        cv::Mat structuringElement = cv::getStructuringElement(cv::MORPH_RECT, cv::Size(3, 3));

        cv::dilate(imgThresh, imgThresh, structuringElement);
        cv::erode(imgThresh, imgThresh, structuringElement);

        // fill circles vector with all circles in processed image
        cv::HoughCircles(imgThresh,			// input image
                         v3fCircles,							// function output (must be a standard template library vector
                         CV_HOUGH_GRADIENT,					// two-pass algorithm for detecting circles, this is the only choice available
                         2,									// size of image / this value = "accumulator resolution", i.e. accum res = size of image / 2
                         imgThresh.rows / 4,				// min distance in pixels between the centers of the detected circles
                         100,								// high threshold of Canny edge detector (called by cvHoughCircles)
                         50,									// low threshold of Canny edge detector (set at 1/2 previous value)
                         10,									// min circle radius (any circles with smaller radius will not be returned)
                         400);								// max circle radius (any circles with larger radius will not be returned)
        for (int i = 0; i < v3fCircles.size(); i++) {		// for each circle . . .
            // show ball position x, y, and radius to command line
            std::cout << "ball position x = " << v3fCircles[i][0]			// x position of center point of circle
                      << ", y = " << v3fCircles[i][1]								// y position of center point of circle
                      << ", radius = " << v3fCircles[i][2] << "\n";				// radius of circle

//            for (int i = 0; i <= 9; i++) // all the rectangles
//            {
//              if( comp_rect.x + comp_rect.width/2 <= region_coordinates[i][2] && comp_rect.x + comp_rect.width/2 >= region_coordinates[i][0] && comp_rect.y + comp_rect.height/2 <= region_coordinates[i][3] && comp_rect.y + comp_rect.height/2 >= region_coordinates[i][1] )
//              {
//                cvRectangle(dst, cvPoint(region_coordinates[i][0],region_coordinates[i][1]), cvPoint(region_coordinates[i][2],region_coordinates[i][3]), CV_RGB(0,0,255), 2, CV_AA, 0 );
//                printf("Detect motion in region %d\n",i);
//              }
//            }

            // draw small green circle at center of detected object
            cv::circle(imgOriginal,												// draw on original image
                       cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
                    3,																// radius of circle in pixels
                    cv::Scalar(0, 255, 0),											// draw pure green (remember, its BGR, not RGB)
                    CV_FILLED);														// thickness, fill in the circle

            // draw red circle around the detected object
            cv::circle(imgOriginal,												// draw on original image
                       cv::Point((int)v3fCircles[i][0], (int)v3fCircles[i][1]),		// center point of circle
                    (int)v3fCircles[i][2],											// radius of circle in pixels
                    cv::Scalar(0, 0, 255),											// draw pure red (remember, its BGR, not RGB)
                    3);																// thickness of circle in pixels
        }	// end for

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
        cv::putText(imgOriginal,
                    "FPS: " + textFps,
                    cv::Point(5, 20),
                    cv::HersheyFonts::FONT_HERSHEY_COMPLEX_SMALL, 1.0,
                    cv::Scalar::all(255));

        //---------------------------------------------------------------------------

        cv::setMouseCallback("Motion", myMouseCallback);
        if (regionCoordinates[digKey][0] != 0 && regionCoordinates[digKey][1] != 0 && regionCoordinates[digKey][2] == 0 && regionCoordinates[digKey][3] == 0)
            cv::rectangle(imgOriginal,
                          cv::Point(regionCoordinates[digKey][0], regionCoordinates[digKey][1]),
                    cv::Point(regionCoordinates[digKey][0]+1, regionCoordinates[digKey][1]+1),
                    cv::Scalar(0,0,255),
                    2,
                    cv::LINE_AA,
                    0);

        if (regionCoordinates[digKey][0] != 0 && regionCoordinates[digKey][1] != 0 && regionCoordinates[digKey][2] != 0 && regionCoordinates[digKey][3] != 0)
            cv::rectangle(imgOriginal,
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
        // declare windows
        cv::namedWindow("Motion", CV_WINDOW_AUTOSIZE);	    // note: you can use CV_WINDOW_NORMAL which allows resizing the window
        cv::namedWindow("imgThresh", CV_WINDOW_AUTOSIZE);	// or CV_WINDOW_AUTOSIZE for a fixed size window matching the resolution of the image
        // CV_WINDOW_AUTOSIZE is the default

        cv::imshow("Motion", imgOriginal);			        // show windows
        cv::imshow("imgThresh", imgThresh);

        //---------------------------------------------------------------------------

        charCheckForEscKey = cv::waitKey(1);
    }
    return 0;
}
