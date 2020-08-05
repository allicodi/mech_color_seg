/*
Mech: Forward Robotics Color Segmentation

Program tracks purple sphero robot in 2D space. Line is drawn to follow path, current coordinates are displayed.
Anticipate use for Tag and Maze activities.

*/

#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;

//Add video name in between | | for pre-recorded video input.
const char* keys = "{ video  | sphero_rolling.mov | Path to the input video file. Skip this argument to capture frames from a camera.}";


int main(int argc, char** argv) {
    // Parse command line arguments
    CommandLineParser parser(argc, argv, keys);

    // Create a VideoCapture object and open the input file

    // FOR CAMERA
    VideoCapture cap;
    
    if (parser.has("video")) {
        cap.open(parser.get<String>("video"));
    }
    else
        cap.open(0);

    // Check if camera opened successfully
    if (!cap.isOpened()) {
        cout << "Error opening video stream or file" << endl;
        return -1;
    } 

    //int frame_width = cap.get(CV_CAP_PROP_FRAME_WIDTH);
    //int frame_height = cap.get(CV_CAP_PROP_FRAME_HEIGHT);

    //VideoWriter video("output.avi", VideoWriter::fourcc('X', '2', '6', '4'), 10, Size(frame_width, frame_height));
    
    int iLastX = -1;
    int iLastY = -1;

    //Capture a temporary image from the camera
    Mat imgTmp;
    cap.read(imgTmp);

    //Create a black image with the size as the camera output
    Mat imgLines = Mat::zeros(imgTmp.size(), CV_8UC3);;

    Mat background;

    for (int i = 0; i < 60; i++)
    {
        cap >> background;
    }

    while (true)
    {
        Mat frame;
        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        //Converting image from BGR to HSV Color space
        Mat hsv;
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Mat mask;

        //HSV Range for purple sphero robot
        inRange(hsv, Scalar(122, 120, 70), Scalar(148, 255, 255), mask);
        
        //Search for purple sphero in frame
        Mat kernel = Mat::ones(3, 3, CV_32F);
        morphologyEx(mask, mask, cv::MORPH_OPEN, kernel);
        morphologyEx(mask, mask, cv::MORPH_DILATE, kernel);

        //Locate center of sphero
        Moments oMoments = moments(mask);

        double dM01 = oMoments.m01;
        double dM10 = oMoments.m10;
        double dArea = oMoments.m00;

        if (dArea > 10000) {
            int posX = dM10 / dArea;
            int posY = dM01 / dArea;

            if (iLastX >= 0 && iLastY >= 0 && posX >= 0 && posY >= 0)
            {
                //Draw a red line from the previous point to the current point
                line(imgLines, Point(posX, posY), Point(iLastX, iLastY), Scalar(0, 0, 255), 2);
                
                //For displaying coordinates on image
                char text[100];
                sprintf_s(text, "x = %d, y = %d", posX, posY);

                rectangle(imgLines, Point(0,0), Point(160,50), Scalar(0, 0, 0), CV_FILLED);
                putText(imgLines, text, Point(0, 50), FONT_HERSHEY_PLAIN, 1.0, CV_RGB(0, 255, 0) );
            }

            iLastX = posX;
            iLastY = posY;

        }

        Mat final_output;
        bitwise_and(frame, frame, final_output, mask);
        
        imshow("Color Segmentation", final_output + imgLines);
        waitKey(1);

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
            break;
        // Also relese all the mat created in the code to avoid memory leakage.
        frame.release(), hsv.release(), mask.release(), final_output.release();

    }

    // When everything done, release the video capture object
    cap.release();
    //video.release();
   
    // Closes all the frames
    destroyAllWindows();

    return 0;
}