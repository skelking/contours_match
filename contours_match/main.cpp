#include <opencv.hpp>
#include <vector>

using namespace cv;
using namespace std;

Mat getDstImage(Mat image)
{
	Mat binary_image;
	int blockSize=15;
	int constValue=3;
	adaptiveThreshold(image,binary_image,255,CV_ADAPTIVE_THRESH_GAUSSIAN_C,CV_THRESH_BINARY,blockSize,constValue);
	//threshold(image,binary_image,12,255,CV_THRESH_BINARY);
	//膨胀处理
	Mat kernal=getStructuringElement(MORPH_RECT,Size(3,3),Point(-1,-1));

	
	erode(binary_image,binary_image,kernal,Point(-1,-1));
	erode(binary_image,binary_image,kernal,Point(-1,-1));
	dilate(binary_image,binary_image,kernal,Point(-1,-1));
	dilate(binary_image,binary_image,kernal,Point(-1,-1));
	imwrite("binary_image.bmp",binary_image);

	return binary_image;
}

//检测符合条件的轮廓数量
void checkContoursNum(vector<vector<Point> > &contours,vector<int> &vecContourIndex,double _a_p)
{
	if(vecContourIndex.size()!=0)
		vecContourIndex.clear();
	for (int idx=0;idx<contours.size();idx++)
	{
		double area=contourArea(contours[idx]);
		double perimeter=arcLength(contours[idx],false);
		double a_p=area/perimeter;

		if (a_p>_a_p)
		{
			vecContourIndex.push_back(idx);
		}
	}
	
	if (vecContourIndex.size()==0)
	{
		checkContoursNum(contours,vecContourIndex,_a_p-0.5);
	}
	else if (vecContourIndex.size()>1)
	{
		checkContoursNum(contours,vecContourIndex,_a_p+0.47);
	}

}

int main()
{

	Mat res_image1=imread("5.bmp");
	if (!res_image1.data)
	{
		return 0;
	}
	Mat gray;
	int W=res_image1.cols;
	int H=res_image1.rows;

	cvtColor(res_image1,gray,CV_BGR2GRAY);
	imwrite("gray.bmp",gray);

	Mat* BGR=new Mat[3];
	split(res_image1,BGR);

	imwrite("blue.bmp",BGR[0]);
	imwrite("green.bmp",BGR[1]);
	imwrite("red.bmp",BGR[2]);

	Mat dst=getDstImage(BGR[2]);

	//查找对应轮廓
	vector<vector<Point> > contours;
	vector<Vec4i> hierarchy;

	findContours( dst, contours, CV_RETR_LIST, CV_CHAIN_APPROX_NONE );

	Mat draw;
	draw=Mat::zeros(dst.size(),CV_8U);
	vector<int> vecContourIndex;
	
	//初步筛选（轮廓周长、面积）
	double minPremiter=(W+H)*2.0*0.1;
	double maxPremiter=(W+H)*2.0*0.9;
	double minArea=(W*H)*0.02;
	double maxArea=(W*H)*0.5;

	double area=0.0;
	double perimeter=0.0;
	vector<vector<Point>> dstContours;
	for (int i=0;i<contours.size();i++)
	{
		area=contourArea(contours[i]);
		perimeter=arcLength(contours[i],false);

		if (area>minArea&&area<maxArea&&perimeter>minPremiter&&perimeter<maxPremiter)
		{
			dstContours.push_back(contours[i]);
		}
	}


	checkContoursNum(dstContours,vecContourIndex,7.5);

	if (vecContourIndex.size()==1)
	{
		drawContours( draw, dstContours, vecContourIndex[0], Scalar(255), CV_FILLED, 8, hierarchy );

		imwrite("draw.bmp",draw);
	}
	
	
	//暂时省去从原图中抠图的过程


	delete[] BGR;
}