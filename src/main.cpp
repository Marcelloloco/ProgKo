#include<stdio.h>
#include<opencv2/opencv.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include<string>
#include<fstream>
#include<chrono>

using namespace cv;
using namespace std::chrono;

//function declaration
Mat applyEmbossFilter(Mat image);

const int filter_width = 3;
const int filter_height = 3;

int filter[filter_width][filter_height] =
{
    -1, -1, 0,
    -1, 0, 1,
    0, 1, 1
};

int main(int argc, char const *argv[])
{
    
    std::string inputs[15];//magic number should be removed in future
    std::ifstream file("../img/img_names.txt");

    if (file.is_open()) 
    {
        int index = 0;

        while (!file.eof() && index < 15) {
            getline(file, inputs[index]);
            ++index;
        }
        file.close();
    }
    else std::cout << "Unable to open the file";

    //start calculation for duration
    auto start = high_resolution_clock::now();

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

        std::cout << "File: " << inputs[i] << std::endl;
        Mat grey_image;
        cvtColor(image, grey_image, COLOR_BGR2GRAY);
        imwrite( "../out/greyscale/" + inputs[i], grey_image);
        Mat hsv_image;
        cvtColor(image, hsv_image, COLOR_BGR2HSV);
        imwrite( "../out/hsv/" + inputs[i], hsv_image);
        imwrite( "../out/emboss/" + inputs[i], applyEmbossFilter(image));
        std::cout << "saved" << std::endl;
    }

    //stop calculation for duration
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    std::cout << "DURATION: " << duration.count() << std::endl;

    return 0;
}


Mat applyEmbossFilter(Mat image)
{
    //initializing a zero Mat array from the input image
    Mat new_image = Mat::zeros( image.size(), image.type() );

    for(int y = 0; y < image.rows; y++) 
    {
        for(int x = 0; x < image.cols; x++) 
        {
            Vec3b intensity = image.at<Vec3b>(y, x);

            double red = 0.0, green = 0.0, blue = 0.0;

            for (size_t i = 0; i < filter_width; i++)
            {
                for (size_t j = 0; j < filter_height; j++)
                {
                    int yPos;
                    yPos = y - (i - 1);
                    if (yPos < 0)
                    {
                        yPos = 0;
                    }
                    else if (yPos > image.rows)
                    {
                        yPos = image.rows;
                    }
                    
                    int xPos;
                    xPos = x - (j - 1);
                    if (xPos < 0)
                    {
                        xPos = 0;
                    }
                    else if (xPos > image.cols)
                    {
                        xPos = image.cols;
                    }
                    
                    red += image.at<Vec3b>(yPos,xPos)[2] * filter[i][j];
                    green += image.at<Vec3b>(yPos,xPos)[1] * filter[i][j];
                    blue += image.at<Vec3b>(yPos,xPos)[0] * filter[i][j];
                }
                
            }

            new_image.at<Vec3b>(y,x)[2] = red;
            new_image.at<Vec3b>(y,x)[1] = green;
            new_image.at<Vec3b>(y,x)[0] = blue;
        }
    }
    return new_image;
}