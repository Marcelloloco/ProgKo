#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<string>
#include<fstream>

using namespace cv;

//function declaration
Mat applyGreyscale(Mat image);

int main(int argc, char const *argv[])
{
    
    std::string inputs[15];//magic number should be removed in future
    std::ifstream file("../img/img_names.txt");

    if (file.is_open()) 
    {
        int index = 0;

        while (!file.eof() && index < 15) {
            getline(file, inputs[index]);
            std::cout << inputs[index] << std::endl;
            ++index;
        }
        file.close();
    }
    else std::cout << "Unable to open the file";

    int input_size = sizeof(inputs)/sizeof(inputs[0]);

    for (size_t i = 0; i < input_size; i++)
    {
        Mat image;
        image = imread("../img/" + inputs[i], IMREAD_COLOR);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }
        namedWindow(inputs[i], WINDOW_AUTOSIZE );
        imshow(inputs[i], applyGreyscale(image));

        waitKey(0);
    }

    return 0;
}

Mat applyGreyscale(Mat image)
{
     Mat new_image = Mat::zeros( image.size(), image.type() );

    for( int y = 0; y < image.rows; y++ ) {
        for( int x = 0; x < image.cols; x++ ) {
            for( int c = 0; c < image.channels(); c++ ) {
                Vec3b intensity = image.at<Vec3b>(y, x);
                new_image.at<Vec3b>(y,x)[c] = 0.21 * intensity.val[0] + 0.72 * intensity.val[1] + 0.07 * intensity.val[0];
            }
        }
    }
    /* For debugging
    int pixelValue = (int)image.at<uchar>(0,0);
    printf("rows: %d col: %d\n", image.rows, image.cols);
    */
    return new_image;
}