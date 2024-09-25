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
            // MySQL 드라이버 객체를 생성하여 데이터베이스와 연결합니다.
            mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
            Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL 서버에 연결
            con->setSchema("open_cv"); // 사용할 데이터베이스 스키마를 설정

            // SQL 쿼리문을 준비합니다.
            std::string query = "SELECT name, width, height, lensNum, color, image FROM phone WHERE lensNum = ? AND color = ?";
            PreparedStatement* pstmt = con->prepareStatement(query);

            // 각 파라미터 값을 설정 (쿼리에서 물음표(?) 순서대로 설정)
            pstmt->setInt(1, lensNum); // 첫 번째 물음표에 lensNum 바인딩
            pstmt->setString(2, detected_color); // 두 번째 물음표에 color 바인딩

            // 쿼리를 실행하고 결과를 조회합니다.
            ResultSet* res = pstmt->executeQuery();
            bool match_found = false;

            while (res->next()) {
                // DB에서 데이터를 가져옵니다.
                double db_width = res->getDouble("width");
                double db_height = res->getDouble("height");
                int db_lensNum = res->getInt("lensNum");
                std::string db_color = res->getString("color");
                std::string db_name = res->getString("name");

                // 오차범위 10%로 설정
                double width_tolerance = db_width * 0.1;
                double height_tolerance = db_height * 0.1;

                // width와 height의 허용 범위 안에 있는지 확인
                bool width_match = (width >= db_width - width_tolerance && width <= db_width + width_tolerance);
                bool height_match = (height >= db_height - height_tolerance && height <= db_height + height_tolerance);

                // 이미지 데이터를 가져와서 `cv::Mat`으로 변환합니다.
                istream* imageBuffer = res->getBlob("image");
                vector<uchar> db_image_data((std::istreambuf_iterator<char>(*imageBuffer)), std::istreambuf_iterator<char>());
                Mat db_image = imdecode(db_image_data, cv::IMREAD_GRAYSCALE); // DB에서 가져온 이미지를 Mat로 변환

                // DB 이미지 유효성 검사
                if (db_image.empty()) {
                    cout << "Error: DB에서 불러온 이미지가 유효하지 않습니다." << endl;
                    continue; // 유효하지 않은 이미지는 스킵
                }

                // 실시간으로 캡처한 이미지 유효성 검사
                if (capturedImage.empty()) {
                    cout << "Error: 캡처한 이미지가 유효하지 않습니다." << endl;
                    return false; // 캡처한 이미지가 유효하지 않으면 종료
                }



                // 실시간으로 캡처한 이미지와 DB 이미지 비교
                bool image_match = Project1::compareImages(db_image, capturedImage); // compareImages 함수 사용

                // 모든 조건이 일치하면 매칭 성공
                if (width_match && height_match && image_match && db_color == detected_color && db_lensNum == lensNum) {
                    matched_name = db_name;
                    matched_width = db_width;
                    matched_height = db_height;
                    matched_lensNum = db_lensNum;
                    matched_color = db_color;
                    match_found = true;
                    // 일치하는 항목을 찾으면 루프를 종료
                }

                if (match_found)
                {
                    cout << "입력한 정보와 일치하는 DB정보를 찾았습니다." << endl;
                }
                else {
                    cout << "입력한 정보와 일치하는 DB정보를 찾지 못했습니다, " << endl;
                }
            }

            // 자원 해제
            delete res;
            delete pstmt;
            delete con;

            // 매칭 결과 반환
            return match_found;
        }
        catch (SQLException& e) {
            std::cout << "SQL 오류: " << e.what() << std::endl;
            return false;
        }
        catch (std::exception& e) {
            std::cout << "오류: " << e.what() << std::endl;
            return false;
        }
    }


