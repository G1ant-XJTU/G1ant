#include<opencv2/opencv.hpp>
#include<opencv2/highgui/highgui.hpp>
#include<opencv2/imgproc/imgproc.hpp>
using namespace std;
using namespace cv;

// BGR
struct BGR
{
    uchar g;
    uchar b;
    uchar r;
};
// HSV
struct HSV
{
    int h;
    double s;
    double v;
};
//WU CHA
bool IsEquals(double val1, double val2)
{
   return fabs(val1 - val2) < 0.001;
}
//RGB TRANS HSV
void COLOR_BGR2HSV(BGR &grb, HSV &hsv)
{
    double g, r, b;
    double h, s, v;
    double min, max;
    double delta;
    
    g = grb.g / 255.0;
    r = grb.r / 255.0;
    b = grb.b / 255.0;
    if(r > g)
    {
        max = MAX(r, b);
        min = MIN(g, b);
    }
    else
	{
		max = MAX(g, b);
		min = MIN(r, b);
	}

	v = max;
	delta = max - min;

	if (IsEquals(max, 0))
		s = 0.0;
	else
		s = delta / max;

	if (max == min)
		h = 0.0;
	else
	{
		if (IsEquals(r, max) && g >= b)
		{
			h = 60 * (g - b) / delta + 0;
		}
		else if (IsEquals(r, max) && g < b)
		{
			h = 60 * (g - b) / delta + 360;
		}
		else if (IsEquals(g, max))
		{
			h = 60 * (b - r) / delta + 120;
		}
		else if (IsEquals(b, max))
		{
			h = 60 * (r - g) / delta + 240;
		}
	}

	hsv.h = (int)(h + 0.5);
	hsv.h = (hsv.h > 359) ? (hsv.h - 360) : hsv.h;
	hsv.h = (hsv.h < 0) ? (hsv.h + 360) : hsv.h;
	hsv.s = s;
	hsv.v = v;

}

int main()
{
    
	Mat image = imread("/home/guoyantong/OpenCV_Project/resources/test_image.png");
    if (image.empty()) {
        cout << "Could not open or find the image" << endl;
        return -1;
    }

    // 图像颜色空间转换
    Mat grayImage, hsvImage;
    cvtColor(image, grayImage, COLOR_BGR2GRAY);
    cvtColor(image, hsvImage, cv::COLOR_BGR2HSV);

    // 均值滤波
    Mat meanBlurImage;
    blur(image, meanBlurImage, Size(5, 5));

    // 高斯滤波
    Mat gaussianBlurImage;
    GaussianBlur(image, gaussianBlurImage, Size(5, 5), 0);

    // 提取红色颜色区域
    Scalar lowerRed1(0, 70, 50), upperRed1(10, 255, 255);
    Scalar lowerRed2(170, 70, 50), upperRed2(180, 255, 255);
    Mat redMask1, redMask2, redMask;
    inRange(hsvImage, lowerRed1, upperRed1, redMask1);
    inRange(hsvImage, lowerRed2, upperRed2, redMask2);
    add(redMask1, redMask2, redMask);

    // 寻找外轮廓
    vector<vector<Point> > contours;
    findContours(redMask, contours, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);

    // 寻找 bounding box
    Rect boundingRect;
    for (size_t i = 0; i < contours.size(); i++) {
        boundingRect = cv::boundingRect(contours[i]);
    }

    // 计算轮廓的面积
    double area = contourArea(contours[0]);

    // 提取高亮颜色区域
    Mat brightImage;
    threshold(grayImage, brightImage, 200, 255, THRESH_BINARY);

    Mat dilatedImage, erodedImage;
    dilate(brightImage, dilatedImage, Mat(), Point(-1, -1), 2);
    erode(dilatedImage, erodedImage, Mat(), Point(-1, -1), 2);

    // 漫水处理
    Mat floodFillImage = erodedImage.clone();
    floodFill(floodFillImage, Point(0, 0), Scalar(255), 0, Scalar(5), Scalar(5), 4);

    // 图像绘制
    int thickness = 2;
    rectangle(image, boundingRect, Scalar(0, 255, 0), thickness);
    drawContours(image, contours, -1, Scalar(0, 0, 255), thickness);
    circle(image, Point(100, 100), 50, Scalar(255, 0, 0), thickness);
    putText(image, "Hello World", Point(50, 50), FONT_HERSHEY_SIMPLEX, 1, Scalar(0, 0, 255), 2);

    // 处理
    Mat rotatedImage;
    Point2f center(image.cols / 2.0F, image.rows / 2.0F);
    Mat rot = getRotationMatrix2D(center, 35, 1.0);
    warpAffine(image, rotatedImage, rot, image.size());

    Mat croppedImage;
    Rect roi(0, 0, image.cols / 4, image.rows / 4);
   	croppedImage = image(roi);

    namedWindow("Original Image", WINDOW_AUTOSIZE);
    imshow("Original Image", image);
	imwrite("OriginalImage.png", image);

    namedWindow("Gray Image", WINDOW_AUTOSIZE);
    imshow("Gray Image", grayImage);
	imwrite("GrayImage.png", grayImage);

    namedWindow("HSV Image", WINDOW_AUTOSIZE);
    imshow("HSV Image", hsvImage);
	imwrite("HSVImage.png", hsvImage);

    namedWindow("Mean Blur Image", WINDOW_AUTOSIZE);
    imshow("Mean Blur Image", meanBlurImage);
	imwrite("MeanImage.png",meanBlurImage);

    namedWindow("Gaussian Blur Image", WINDOW_AUTOSIZE);
    imshow("Gaussian Blur Image", gaussianBlurImage);
	imwrite("GaussianImage.png", gaussianBlurImage);

    namedWindow("Red Mask", WINDOW_AUTOSIZE);
    imshow("Red Mask", redMask);
	imwrite("RedImage.png", redMask);

    namedWindow("Bright Image", WINDOW_AUTOSIZE);
    imshow("Bright Image", brightImage);
	imwrite("BrightImage.png", brightImage);

    namedWindow("Dilated Image", WINDOW_AUTOSIZE);
    imshow("Dilated Image", dilatedImage);
	imwrite("DilatedImage.png", dilatedImage);

    namedWindow("Eroded Image", WINDOW_AUTOSIZE);
    imshow("Eroded Image", erodedImage);
	imwrite("ErodedImage.png",erodedImage);

    namedWindow("Flood Fill Image", WINDOW_AUTOSIZE);
    imshow("Flood Fill Image", floodFillImage);
	imwrite("FloodFillImage.png", floodFillImage);

    namedWindow("Rotated Image", WINDOW_AUTOSIZE);
    imshow("Rotated Image", rotatedImage);
	imwrite("RotatedImage.png", rotatedImage);

    namedWindow("Cropped Image", WINDOW_AUTOSIZE);
    imshow("Cropped Image", croppedImage);
	imwrite("CroppedImage.png", croppedImage);

    waitKey(0);
    return 0;
}
