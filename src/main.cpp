#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<string>
#include<fstream>

using namespace cv;

//function declaration
Mat applyGreyscale(Mat image);
Mat convertBGR2HSV(Mat image);

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
        Mat gray_image = applyGreyscale(image);
        imwrite( "../out/" + inputs[i], gray_image );
        //std::cout << inputs[i] << std::endl;
        imwrite( "../out/hsv/" + inputs[i], convertBGR2HSV(image) );
        namedWindow(inputs[i], WINDOW_AUTOSIZE );
        imshow(inputs[i], convertBGR2HSV(image));

        waitKey(0);
    }

    return 0;
}

Mat applyGreyscale(Mat image)
{
    //initializing a zero Mat array from the input image
    Mat new_image = Mat::zeros( image.size(), image.type() );

    for( int y = 0; y < image.rows; y++ ) {
        for( int x = 0; x < image.cols; x++ ) {
            for( int c = 0; c < image.channels(); c++ ) {
                Vec3b intensity = image.at<Vec3b>(y, x);
                new_image.at<Vec3b>(y,x)[c] = 0.07 * intensity.val[0] + 0.72 * intensity.val[1] + 0.21 * intensity.val[2];
            }
        }
    }

    return new_image;
}

Mat convertBGR2HSV(Mat image)
{
    //initializing a zero Mat array from the input image
    Mat new_image = Mat::zeros( image.size(), image.type());

    for( int y = 0; y < image.rows; y++ ) {
        for( int x = 0; x < image.cols; x++ ) {
            Vec3b intensity = image.at<Vec3b>(y, x);
            int h;
            int s;
            int v = 0;
            
            float r = intensity.val[2] / (float)255;
            float g = intensity.val[1] / (float)255;
            float b = intensity.val[0] / (float)255;
            
            float cmax = max(max(r,g), b);
            float cmin = min(min(r,g), b);
            float diff = cmax - cmin;

            if(cmax == cmin)
            {
                h = 0;
            } 
            else if (cmax == r) 
            {
                h = ((int )(60 * ((g - b) / diff) + 360)) % 360;
            } 
            else if (cmax == g) 
            {
                h = ((int )(60 * ((b - r) / diff) + 120)) % 360;
            } 
            else if (cmax == b) 
            {
                h = ((int ) (60 * ((r - g) / diff) + 240)) % 360;
            }
            
            if (cmax == 0)
            {
                s = 0;
            }
            else
            {
                s = (diff / (float)cmax) * 100;
            }
            
            v = cmax * 100;
            
            intensity.val[0] = h;
            intensity.val[1] = s;
            intensity.val[2] = v;
            
            new_image.at<Vec3b>(y,x) = intensity;
        }
    }

    return new_image;
}