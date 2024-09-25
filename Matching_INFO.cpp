#include "Form1.h"
#include "DB_head.h"
#include "image_search.h"
#include <opencv2/opencv.hpp>
#include <opencv2/features2d.hpp>
#include <iostream>
#include <numeric>



using namespace cv;
using namespace std;

    bool Matching_INFO(const std::string& name, double width, double height, int lensNum, std::string& detected_color, const std::vector<uchar>& imageBuffer, cv::Mat& capturedImage, std::string& matched_name, double& matched_width, double& matched_height, int& matched_lensNum, std::string& matched_color) {
        try {
            // MySQL ����̹� ��ü�� �����Ͽ� �����ͺ��̽��� �����մϴ�.
            mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
            Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL ������ ����
            con->setSchema("open_cv"); // ����� �����ͺ��̽� ��Ű���� ����

            // SQL �������� �غ��մϴ�.
            std::string query = "SELECT name, width, height, lensNum, color, image FROM phone WHERE lensNum = ? AND color = ?";
            PreparedStatement* pstmt = con->prepareStatement(query);

            // �� �Ķ���� ���� ���� (�������� ����ǥ(?) ������� ����)
            pstmt->setInt(1, lensNum); // ù ��° ����ǥ�� lensNum ���ε�
            pstmt->setString(2, detected_color); // �� ��° ����ǥ�� color ���ε�

            // ������ �����ϰ� ����� ��ȸ�մϴ�.
            ResultSet* res = pstmt->executeQuery();
            bool match_found = false;

            while (res->next()) {
                // DB���� �����͸� �����ɴϴ�.
                double db_width = res->getDouble("width");
                double db_height = res->getDouble("height");
                int db_lensNum = res->getInt("lensNum");
                std::string db_color = res->getString("color");
                std::string db_name = res->getString("name");

                // �������� 10%�� ����
                double width_tolerance = db_width * 0.1;
                double height_tolerance = db_height * 0.1;

                // width�� height�� ��� ���� �ȿ� �ִ��� Ȯ��
                bool width_match = (width >= db_width - width_tolerance && width <= db_width + width_tolerance);
                bool height_match = (height >= db_height - height_tolerance && height <= db_height + height_tolerance);

                // �̹��� �����͸� �����ͼ� `cv::Mat`���� ��ȯ�մϴ�.
                istream* imageBuffer = res->getBlob("image");
                vector<uchar> db_image_data((std::istreambuf_iterator<char>(*imageBuffer)), std::istreambuf_iterator<char>());
                Mat db_image = imdecode(db_image_data, cv::IMREAD_GRAYSCALE); // DB���� ������ �̹����� Mat�� ��ȯ

                // DB �̹��� ��ȿ�� �˻�
                if (db_image.empty()) {
                    cout << "Error: DB���� �ҷ��� �̹����� ��ȿ���� �ʽ��ϴ�." << endl;
                    continue; // ��ȿ���� ���� �̹����� ��ŵ
                }

                // �ǽð����� ĸó�� �̹��� ��ȿ�� �˻�
                if (capturedImage.empty()) {
                    cout << "Error: ĸó�� �̹����� ��ȿ���� �ʽ��ϴ�." << endl;
                    return false; // ĸó�� �̹����� ��ȿ���� ������ ����
                }



                // �ǽð����� ĸó�� �̹����� DB �̹��� ��
                bool image_match = Project1::compareImages(db_image, capturedImage); // compareImages �Լ� ���

                // ��� ������ ��ġ�ϸ� ��Ī ����
                if (width_match && height_match && image_match && db_color == detected_color && db_lensNum == lensNum) {
                    matched_name = db_name;
                    matched_width = db_width;
                    matched_height = db_height;
                    matched_lensNum = db_lensNum;
                    matched_color = db_color;
                    match_found = true;
                    // ��ġ�ϴ� �׸��� ã���� ������ ����
                }

                if (match_found)
                {
                    cout << "�Է��� ������ ��ġ�ϴ� DB������ ã�ҽ��ϴ�." << endl;
                }
                else {
                    cout << "�Է��� ������ ��ġ�ϴ� DB������ ã�� ���߽��ϴ�, " << endl;
                }
            }

            // �ڿ� ����
            delete res;
            delete pstmt;
            delete con;

            // ��Ī ��� ��ȯ
            return match_found;
        }
        catch (SQLException& e) {
            std::cout << "SQL ����: " << e.what() << std::endl;
            return false;
        }
        catch (std::exception& e) {
            std::cout << "����: " << e.what() << std::endl;
            return false;
        }
    }


