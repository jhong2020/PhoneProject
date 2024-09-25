#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <numeric>
#include "image_search.h" 
#include "Form1.h"
using namespace cv;
using namespace std;

namespace Project1 {
    // 이미지가 유효한지 확인하는 함수
    bool isValidImage1(const Mat& imgtemplate) {
        return !imgtemplate.empty();
    }
    bool isValidImage2(const Mat& capturedImage) {
        return !capturedImage.empty();
    }

    // ORB(Oriented FAST and Rotated BRIEF)를 사용하여 특징점과 디스크립터를 추출하는 함수
    void extractFeatures(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors, Ptr<ORB>& orb) {
        orb->detectAndCompute(img, Mat(), keypoints, descriptors);
    }

    // 두 이미지의 디스크립터를 매칭하는 함수
    vector<DMatch> matchFeatures(const Mat& descriptors1, const Mat& descriptors2, BFMatcher& matcher) {
        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);  // 디스크립터 간 매칭 수행
        return matches;  // 매칭 결과 반환
    }

    // 이미지 비교 함수: DB에서 불러온 이미지와 실시간으로 캡처한 이미지 비교
    bool compareImages(const Mat& imgTemplate, const Mat& capturedImage) {

        //이미지 유효성 검사
        /*if (!isValidImage1(imgTemplate)) {
            cout << "Error: Invalid image data. 1" << endl;
            return false;
        }
        else if (!isValidImage2(capturedImage)) {
            cout << "Error: Invalid image data. 2" << endl;
            return false;
        }*/

        Ptr<ORB> orb1 = ORB::create();
        Ptr<ORB> orb2 = ORB::create();
        vector<KeyPoint> keypointsTemplate, keypointsCaptured;
        Mat descriptorsTemplate, descriptorsCaptured;

        // 템플릿 이미지와 캡처된 이미지의 특징점 및 디스크립터 추출
        extractFeatures(imgTemplate, keypointsTemplate, descriptorsTemplate, orb1);
        extractFeatures(capturedImage, keypointsCaptured, descriptorsCaptured, orb2);

        // 디스크립터가 비어있는지 확인
        /*if (descriptorsTemplate.empty()) {
            cout << "Error: Failed to extract descriptors.1" << endl;
            return false;
        }
        else if (descriptorsCaptured.empty()) {
            cout << "Error: Failed to extract descriptors.2" << endl;
            return false;
        }*/


        // BFMatcher를 사용한 매칭
        BFMatcher matcher(NORM_HAMMING);
        vector<DMatch> matches = matchFeatures(descriptorsTemplate, descriptorsCaptured, matcher);

        // 매칭된 특징점이 있을 경우 일치율 계산
        if (!matches.empty()) {
            // 매칭된 특징점들의 거리 합계를 계산
            double sumDistances = accumulate(matches.begin(), matches.end(), 0.0, [](double acc, const DMatch& match) {
                return acc + match.distance;
                });

            // 평균 거리 계산
            double avgDistance = sumDistances / matches.size();

            // 최대 거리를 100.0으로 설정하여 일치율 계산
            double maxDistance = 100.0;
            double matchRatio = (1.0 - avgDistance / maxDistance) * 100.0;

            // 일치율 출력
            

            // 일치율이 60% 이상이면 true 반환
            if (matchRatio >= 60) {
               cout<<  "일치율: " << matchRatio << "%" << endl;
                return true;
            }
            else {
                return false;
            }
        }

        // 매칭된 특징점이 없을 경우 false 반환
        return false;
    }
}