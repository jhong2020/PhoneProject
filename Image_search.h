#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <numeric>  // accumulate 함수에 필요한 헤더
using namespace cv;
using namespace std;


namespace Project1 {
	// 두 점 사이의 유클리드 거리(Euclidean distance)를 계산하는 함수
	//double getDistance(Point2f p1, Point2f p2);
	// ORB(Oriented FAST and Rotated BRIEF)를 사용하여 특징점과 디스크립터를 추출하는 함수
	void extractFeatures(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors, Ptr<ORB>& orb);
	// 두 이미지의 디스크립터를 매칭하는 함수

	vector<DMatch> matchFeatures(const Mat& descriptors1, const Mat& descriptors2, BFMatcher& matcher);
	// 이미지 비교 함수: 입력된 이미지와 실시간 웹캠 영상의 유사도를 계산하여 일치 여부를 반환

	bool compareImages(const Mat& imgTemplate, const Mat& capturedImage);
	// MySQL에서 모든 이미지를 BLOB으로 가져와서 벡터에 저장하는 함수
	//vector<Mat> getAllImagesFromDatabase(const string& query);
	//// 모든 데이터베이스 이미지와 실시간 웹캠 프레임을 비교하는 함수
	//bool compareImagesFromDatabase(VideoCapture& cap, const string& query);

}