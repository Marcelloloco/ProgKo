#include<stdio.h>
#include<opencv2/opencv.hpp>
#include<string>
#include<fstream>
#include<mpi.h>
#include<chrono>

using namespace cv;
using namespace std::chrono;

//function declaration
Mat applyGreyscale(Mat image);
Mat convertBGR2HSV(Mat image);
Mat applyEmbossFilter(Mat image);

const int filter_width = 3;
const int filter_height = 3;

int filter[filter_width][filter_height] =
{
    -1, -1, 0,
    -1, 0, 1,
    0, 1, 1
};

int main(int argc, char *argv[])
{
    //number of processes
    int size;
    int rank;

    std::string inputs[15];//magic number should be removed in future
    int indices[15] = {0,1,2,3,4,5,6,7,8,9,10,11,12,13,14};
    int partBuffer[4];

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

    MPI_Init(&argc, &argv);

    MPI_Comm_size(MPI_COMM_WORLD, &size);
    MPI_Comm_rank(MPI_COMM_WORLD, &rank);
    
    MPI_Barrier(MPI_COMM_WORLD);
    MPI_Scatter(&indices, 5, MPI_INT, &partBuffer, 5, MPI_INT, 0, MPI_COMM_WORLD);

    int input_size = sizeof(partBuffer)/sizeof(partBuffer[0]);
    int workload[input_size];
    //initialization of workload buffer because of a bug
    for (size_t i = 0; i <= input_size; i++)
    {
        workload[i] = partBuffer[i];
    }

    for (size_t i = 0; i <= input_size; i++)
    {
        int index = workload[i];
        Mat image;
        image = imread("../img/" + inputs[index], IMREAD_COLOR);
        if ( !image.data )
        {
            printf("No image data \n");
            return -1;
        }

        std::cout << "rank: " << rank << " index: " << index  << " File: " << inputs[index] << std::endl;
        imwrite( "../out/greyscale/" + inputs[index], applyGreyscale(image));
        imwrite( "../out/hsv/" + inputs[index], convertBGR2HSV(image));
        imwrite( "../out/emboss/" + inputs[index], applyEmbossFilter(image));
        std::cout << "saved" << std::endl;
    }

    MPI_Finalize();

    //stop calculation for duration
    auto stop = high_resolution_clock::now();
    auto duration = duration_cast<microseconds>(stop - start); 
    std::cout << "DURATION: " << duration.count() << std::endl;

    return 0;
}

Mat applyGreyscale(Mat image)
{
    //initializing a zero Mat array from the input image
    Mat new_image = Mat::zeros( image.size(), image.type() );

    for(int y = 0; y < image.rows; y++) 
    {
        for(int x = 0; x < image.cols; x++) 
        {
            for(int c = 0; c < image.channels(); c++) 
            {
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

    for(int y = 0; y < image.rows; y++) 
    {
        for(int x = 0; x < image.cols; x++) 
        {
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