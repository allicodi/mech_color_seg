#include "opencv2/opencv.hpp"
#include "opencv2/imgproc/imgproc.hpp"
#include "opencv2/highgui/highgui.hpp"
#include <iostream>

using namespace std;
using namespace cv;


const char* keys = "{ video  |  | Path to the input video file. Skip this argument to capture frames from a camera.}";


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

    int frame_width = static_cast<int>(cap.get(CAP_PROP_FRAME_WIDTH));
    printf("%d\n", frame_width);
    int frame_height = static_cast<int>(cap.get(CAP_PROP_FRAME_HEIGHT));
    printf("%d\n", frame_height);
    Size frame_size(frame_width, frame_height);
    int frames_per_second = 10;

    Mat background;
    for (int i = 0; i < 60; i++)
    {
        cap >> background;
    }

    //flip(background,background,1);
    const String name = "output.avi";

    //VideoWriter oVideoWriter("output.avi", CV_FOURCC('M','P', '4', 'G'), frames_per_second, (288,512), true);
    //
    //// If the VideoWriter object is not initialized successfully, exit the program
    //if (oVideoWriter.isOpened() == false)
    //{
    //    cout << "Cannot save the video to a file" << endl;
    //    cin.get(); //wait for any key press
    //    return -1;
    //}

    while (1)
    {

        Mat frame;
        // Capture frame-by-frame
        cap >> frame;

        // If the frame is empty, break immediately
        if (frame.empty())
            break;

        //Converting image from BGR to HSV Color space
        Mat hsv;
        //flip(frame,frame,1);
        cvtColor(frame, hsv, COLOR_BGR2HSV);

        Mat mask1, mask2;

        //SAMPLE FOR RED
        //Create mask to detect the upper and lower red color
        //inRange(hsv, Scalar(0, 120, 70), Scalar(10, 255, 255), mask1);
        //inRange(hsv, Scalar(170, 120, 70), Scalar(180, 255, 255), mask2);

        //ADJUSTED FOR PURPLE 
        //FIXME: double masks unnecessary, only for red, not causing problems at the moment
        inRange(hsv, Scalar(122, 120, 70), Scalar(148, 255, 255), mask1);
        inRange(hsv, Scalar(122, 120, 70), Scalar(128, 255, 255), mask2);

        //Generate final mask
        mask1 = mask1 + mask2;

        Mat kernel = Mat::ones(3, 3, CV_32F);
        morphologyEx(mask1, mask1, cv::MORPH_OPEN, kernel);
        morphologyEx(mask1, mask1, cv::MORPH_DILATE, kernel);

        bitwise_not(mask1, mask2);

        Mat res1, res2, final_output;
        bitwise_and(frame, frame, res1, mask2);
        bitwise_and(background, background, res2, mask1);
        addWeighted(res1, 1, res2, 1, 0, final_output);

        imshow("Color Segmentation", final_output);
        waitKey(1);

        // Display the resulting frame
        //imshow( "Frame", frame );

        // Press  ESC on keyboard to exit
        char c = (char)waitKey(25);
        if (c == 27)
            break;
        // Also relese all the mat created in the code to avoid memory leakage.
        frame.release(), hsv.release(), mask1.release(), mask2.release(), res1.release(), res2.release(), final_output.release();

    }


    // When everything done, release the video capture object
    cap.release();
    //oVideoWriter.release();

    // Closes all the frames
    destroyAllWindows();

    return 0;
}