#include <opencv2/imgcodecs.hpp>
#include<opencv2/highgui.hpp>
#include<opencv2/imgproc.hpp>
#include<opencv2/objdetect.hpp>
#include <iostream>
using namespace cv;
using namespace std;
/// <summary>
/// not summery bs ma4i
/// </summary>

Point drawleft(Mat img,Mat output,Rect face)
{

	int x = face.x;
	vector<Point> linee;
	bool stop = false;
	Point start(0,0);
	int prev = 0;
	while (x!=0) {
		for (int y = face.br().y-5; y < img.size().height; y++)
		{
			if (img.at<Vec3b>(Point(x, y))[0] == 0 && img.at<Vec3b>(Point(x, y))[1] == 0 && img.at<Vec3b>(Point(x, y))[2] == 254)
			{
				if (start == Point(0, 0))
				{
					start = Point(x, y);
				}
				if(start.y==face.br().y)
					start = Point(x, y);

				if (y > prev)
				{
					linee.push_back(Point(x, y));
					prev = y;
				}
				
				break;
			}
		}
		if (x < (face.x - 80))
			break;
		x--;
	}
	Vec4f linedata;
	if (linee.size() > 1)
	{
		fitLine(linee, linedata, DIST_L2, 0, 0.01, 0.01);
		line(output, start, Point(start.x - 1000 * linedata(0), start.y - 1000 * linedata(1)), Scalar(0, 0, 0), 3);
		return Point(start.x + 500 * linedata(0), start.y + 500 * linedata(1));
	}
	return Point(0,0);


}
Point drawright(Mat img, Mat output,Rect face)
{

	int x = face.br().x;
	vector<Point> linee;
	bool stop = false;
	Point start(0, 0);
	int prev = 0;
	while (x != img.size().width) {
		for (int y = face.br().y-5; y < img.size().height; y++)
		{
			if (img.at<Vec3b>(Point(x, y))[0] == 0 && img.at<Vec3b>(Point(x, y))[1] == 0 && img.at<Vec3b>(Point(x, y))[2] == 254)
			{
				if (start == Point(0, 0))
				{
					start = Point(x, y);
				}
				if (start.y == face.br().y)
					start = Point(x, y);

				if (y > prev)
				{
					linee.push_back(Point(x, y));
					prev = y;
				}

				break;
			}
		}
		if (x >(face.x +80+face.width))
			break;

		x++;
	}
	Vec4f linedata;
	cout << start.x << " " << start.y << "////////////////" << endl;
	if (linee.size() > 1)
	{
		fitLine(linee, linedata, DIST_L2, 0, 0.01, 0.01);
		line(output, start, Point(start.x + 1000 * linedata(0), start.y + 1000 * linedata(1)), Scalar(0, 0, 0), 3);
		return Point(start.x + 500 * linedata(0), start.y + 500 * linedata(1));
	}
	return Point(0, 0);



}






void getcontours(Mat imgdil,Mat imgout)
{
	vector<vector<Point>> contours;
	vector<Vec4i> hierarchy;
	findContours(imgdil, contours, hierarchy, RETR_EXTERNAL, CHAIN_APPROX_SIMPLE);
	
	int area;
	float peri;
	vector<vector<Point>> conpoly(contours.size());
	for (int i = 0; i < contours.size(); i++)
	{
		area = contourArea(contours[i]);
	
		cout << area << endl;
		if (area > 3500)
		{
			peri = arcLength(contours[i], 1);
			approxPolyDP(contours[i], conpoly[i], 0.002*peri, 1);
			area = contourArea(conpoly[i]);
			drawContours(imgout, conpoly, i, Scalar(0, 0, 254), 2);
		}
	}
}

void main()
{
	Mat imggray, imgblur, imgcanny, imgdil, imgerode, imgcroped,kernel,output;
	VideoCapture cap(0);
	Rect partused;
Mat img;
CascadeClassifier facecascade;
facecascade.load("Resources/haarcascade_frontalface_default.xml");
if (facecascade.empty())
cout << "xml file failed" << endl;
vector<Rect> faces;
int count=0;
while (1)
{
	 cap.read(img);
	 //cap.read(output);
	 flip(img, img, 1);
	 line(img, Point(0, img.size().height), Point(img.size().width, img.size().height), Scalar(250, 0, 250), 3);
	 facecascade.detectMultiScale(img, faces, 1.1, 10);
	 for (int i = 0; i < faces.size(); i++)
	 {
		rectangle(img, faces[i].tl(), faces[i].br(), Scalar(0, 0, 120), 4);

	 }
	 imshow("Image", img);
	 waitKey(1);
		
	
    		if (faces.size() >= 1)
		    {	
			output = img;
    		partused = Rect(5, faces[0].br().y - 10, img.size().width-5, img.size().height + 10 - faces[0].br().y);
			
    		imgcroped = img(partused);
		    cvtColor(imgcroped, imggray, COLOR_BGR2GRAY);
		    GaussianBlur(imggray, imgblur, Size(3, 3), 3, 0);
		    Canny(imgblur, imgcanny, 25, 75);
		    kernel = getStructuringElement(MORPH_RECT, Size(3, 3));
		    dilate(imgcanny, imgdil, kernel);
		    getcontours(imgdil, imgcroped);
		    Point leftshlder = drawleft(img, output,faces[0]);
    		Point rightshlder = drawright(img, output,faces[0]);
			if (!(leftshlder.y <= (rightshlder.y +40) && leftshlder.y >= (rightshlder.y - 40)))
				count++;
			else
				count = 0;
			if (count == 5)
			{
				count = 0;
			}
	        cv::imshow("imgafter", output);
	        }
	
}
}

