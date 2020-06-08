#include <opencv2/highgui.hpp>

int main(int argc, char const *argv[])
{
    cv::Mat image;

    image = cv::imread("../img/animal/1.kitten_small.jpg", cv::IMREAD_COLOR);
    cv::imshow("TEST", image);
    cv::waitKey(0);
    return 0;
}
