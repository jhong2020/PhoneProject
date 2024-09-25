#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <numeric>
#include "image_search.h" 
#include "Form1.h"
using namespace cv;
using namespace std;

namespace Project1 {
    // �̹����� ��ȿ���� Ȯ���ϴ� �Լ�
    bool isValidImage1(const Mat& imgtemplate) {
        return !imgtemplate.empty();
    }
    bool isValidImage2(const Mat& capturedImage) {
        return !capturedImage.empty();
    }

    // ORB(Oriented FAST and Rotated BRIEF)�� ����Ͽ� Ư¡���� ��ũ���͸� �����ϴ� �Լ�
    void extractFeatures(const Mat& img, vector<KeyPoint>& keypoints, Mat& descriptors, Ptr<ORB>& orb) {
        orb->detectAndCompute(img, Mat(), keypoints, descriptors);
    }

    // �� �̹����� ��ũ���͸� ��Ī�ϴ� �Լ�
    vector<DMatch> matchFeatures(const Mat& descriptors1, const Mat& descriptors2, BFMatcher& matcher) {
        vector<DMatch> matches;
        matcher.match(descriptors1, descriptors2, matches);  // ��ũ���� �� ��Ī ����
        return matches;  // ��Ī ��� ��ȯ
    }

    // �̹��� �� �Լ�: DB���� �ҷ��� �̹����� �ǽð����� ĸó�� �̹��� ��
    bool compareImages(const Mat& imgTemplate, const Mat& capturedImage) {

        //�̹��� ��ȿ�� �˻�
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

        // ���ø� �̹����� ĸó�� �̹����� Ư¡�� �� ��ũ���� ����
        extractFeatures(imgTemplate, keypointsTemplate, descriptorsTemplate, orb1);
        extractFeatures(capturedImage, keypointsCaptured, descriptorsCaptured, orb2);

        // ��ũ���Ͱ� ����ִ��� Ȯ��
        /*if (descriptorsTemplate.empty()) {
            cout << "Error: Failed to extract descriptors.1" << endl;
            return false;
        }
        else if (descriptorsCaptured.empty()) {
            cout << "Error: Failed to extract descriptors.2" << endl;
            return false;
        }*/


        // BFMatcher�� ����� ��Ī
        BFMatcher matcher(NORM_HAMMING);
        vector<DMatch> matches = matchFeatures(descriptorsTemplate, descriptorsCaptured, matcher);

        // ��Ī�� Ư¡���� ���� ��� ��ġ�� ���
        if (!matches.empty()) {
            // ��Ī�� Ư¡������ �Ÿ� �հ踦 ���
            double sumDistances = accumulate(matches.begin(), matches.end(), 0.0, [](double acc, const DMatch& match) {
                return acc + match.distance;
                });

            // ��� �Ÿ� ���
            double avgDistance = sumDistances / matches.size();

            // �ִ� �Ÿ��� 100.0���� �����Ͽ� ��ġ�� ���
            double maxDistance = 100.0;
            double matchRatio = (1.0 - avgDistance / maxDistance) * 100.0;

            // ��ġ�� ���
            

            // ��ġ���� 60% �̻��̸� true ��ȯ
            if (matchRatio >= 60) {
               cout<<  "��ġ��: " << matchRatio << "%" << endl;
                return true;
            }
            else {
                return false;
            }
        }

        // ��Ī�� Ư¡���� ���� ��� false ��ȯ
        return false;
    }
}