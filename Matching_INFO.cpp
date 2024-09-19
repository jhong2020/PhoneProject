#include "DB_head.h" // 헤더파일 필요시 변경
#include "Form1.h"
// 주어진 핸드폰 정보가 데이터베이스에 존재하는지 확인하는 함수
bool Matching_INFO(double width,  double height, int lensNum, const string& detected_color, const std::vector<uchar>& imageBuffer) {
    try {
        // MySQL 드라이버 객체를 생성하여 데이터베이스와 연결합니다.
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL 서버에 연결 // DB 주소, 이름, 패스워드 변경
        con->setSchema("open_cv"); // 사용할 데이터베이스 스키마를 설정

        // 특정 핸드폰의 정보를 조회하기 위한 SQL 쿼리문을 준비합니다.
        // 필요시 쿼리문 변경
        string query = "SELECT width, height, lensNum,color,image FROM phone WHERE name = ?"; // 4개의 열을 조회하고, name열의 값이 일치하는 행만 조회
        PreparedStatement* pstmt = con->prepareStatement(query);

        //pstmt->setDouble(1, width); // 쿼리의 파라미터에 핸드폰 이름을 바인딩

        // 쿼리를 실행하고 결과를 조회합니다.
        ResultSet* res = pstmt->executeQuery();
        bool match_found = false;

        // 결과가 존재하는지 확인하고, 입력된 정보와 데이터베이스의 정보를 비교합니다.
        if (res->next()) {
            double db_width= res->getDouble("width");
            double db_height= res->getDouble("height");
            int db_lensNum = res->getInt("lensNum");
            string db_color = res->getString("color");
            /*std::istream* db_image = res->getBlob("image");*/
            // 입력된 정보와 데이터베이스에서 조회한 정보를 비교하여 일치 여부를 확인합니다.
            match_found = (lensNum == db_lensNum) && (height == db_height) &&
                (width == db_width) && (detected_color == db_color);

            // 정보가 일치할 경우 데이터베이스에서 조회한 정보를 출력합니다.
            if (match_found) {
                cout << "입력한 정보와 일치하는 DB 정보를 찾았습니다:" << endl;
                //cout << "기종: " << name << endl;
                cout << "카메라 개수: " << db_lensNum << endl;
                cout << "높이(mm): " << db_height << endl;
                cout << "너비(mm): " << db_width << endl;
                cout << "색상: " << db_color << endl;

            }
        }

        // 사용한 자원(쿼리 결과, 준비된 문장, 데이터베이스 연결)을 해제합니다.
        delete res;
        delete pstmt;
        delete con;

        // 정보가 일치하면 true를 반환, 그렇지 않으면 false를 반환합니다.
        return match_found;
    }
    catch (SQLException& e) {
        // SQL 예외가 발생한 경우 예외 메시지를 출력하고 false를 반환합니다.
        cout << "SQL 오류: " << e.what() << endl;
        return false;
    }
    catch (exception& e) {
        // 일반 예외가 발생한 경우 예외 메시지를 출력하고 false를 반환합니다.
        cout << "오류: " << e.what() << endl;
        return false;
    }
}
