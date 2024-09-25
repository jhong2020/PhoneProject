#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <numeric>  // accumulate �Լ��� �ʿ��� ���
using namespace cv;
using namespace std;


namespace Project1 {
	// �� �� ������ ��Ŭ���� �Ÿ�(Euclidean distance)�� ����ϴ� �Լ�
	//double getDistance(Point2f p1, Point2f p2);
	// ORB(Oriented FAST and Rotated BRIEF)�� ����Ͽ� Ư¡���� ��ũ���͸� �����ϴ� �Լ�
	void extractFeatures(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors, Ptr<ORB>& orb);
	// �� �̹����� ��ũ���͸� ��Ī�ϴ� �Լ�

	vector<DMatch> matchFeatures(const Mat& descriptors1, const Mat& descriptors2, BFMatcher& matcher);
	// �̹��� �� �Լ�: �Էµ� �̹����� �ǽð� ��ķ ������ ���絵�� ����Ͽ� ��ġ ���θ� ��ȯ

	bool compareImages(const Mat& imgTemplate, const Mat& capturedImage);
	// MySQL���� ��� �̹����� BLOB���� �����ͼ� ���Ϳ� �����ϴ� �Լ�
	//vector<Mat> getAllImagesFromDatabase(const string& query);
	//// ��� �����ͺ��̽� �̹����� �ǽð� ��ķ �������� ���ϴ� �Լ�
	//bool compareImagesFromDatabase(VideoCapture& cap, const string& query);

}