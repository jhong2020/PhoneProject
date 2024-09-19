#include "DB_head.h" // 헤더파일 필요시 변경
#include<vector>

// 데이터베이스에 핸드폰 정보를 입력하는 함수
int get_INFO(double width, double height, int lensNum, const string& detected_color, const std::vector<uchar>& imageBuffer) {
    try {
        // MySQL 드라이버 객체를 생성하여 데이터베이스와 연결합니다.
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        // 서버 주소 ~ 이름, 패스워드 변경해야함 
        Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL 서버에 연결
        con->setSchema("open_cv"); // 사용할 데이터베이스 스키마를 설정

        // 사용자로부터 핸드폰 정보를 입력 받습니다.
        //string name, color;
        //int camera_num, height, width;

        //cout << "핸드폰 이름을 입력하세요: ";
        //cin.ignore(); // 이전 입력에서 남은 '\n'을 제거
        //getline(cin, name); // 핸드폰 이름 입력 받기

        //// 카메라 개수 입력
        //cout << "카메라 개수: ";
        //while (!(cin >> camera_num) || camera_num <= 0) {
        //    cin.clear(); // 입력 스트림 오류 플래그를 지움
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남은 입력을 제거
        //    cout << "다시 입력하세요: " << endl;
        //}

        //// 핸드폰 높이 입력
        //cout << "높이(mm): ";
        //while (!(cin >> height) || height <= 0) {
        //    cin.clear(); // 입력 스트림 오류 플래그를 지움
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남은 입력을 제거
        //    cout << "잘못된 입력입니다. " << endl;
        //}

        //// 핸드폰 너비 입력
        //cout << "너비(mm): ";
        //while (!(cin >> width) || width <= 0) {
        //    cin.clear(); // 입력 스트림 오류 플래그를 지움
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남은 입력을 제거
        //    cout << "잘못된 입력입니다. " << endl;
        //}

        //cin.ignore(); // 이전 입력에서 남은 '\n'을 제거

        //// 핸드폰 색상 입력
        //cout << "색상을 입력하세요: ";
        //getline(cin, color); // 색상 입력 받기

        std::stringstream ss;
        ss.write(reinterpret_cast<const char*>(imageBuffer.data()), imageBuffer.size());
        

        // 변수 이름 필요시 변경
        // 핸드폰 정보를 데이터베이스에 삽입하기 위한 SQL 쿼리문을 준비합니다.
        // 쿼리문 필요시 변환

        string query = "INSERT INTO phone ( width, height, lensNum, color, image) VALUES (?, ?, ?, ?, ?)";
        PreparedStatement* pstmt = con->prepareStatement(query);
        pstmt->setDouble(1, width); 
        pstmt->setDouble(2, height); 
        pstmt->setInt(3, lensNum); 
        pstmt->setString(4, detected_color);
        pstmt->setBlob(5, &ss); 

        // 데이터베이스에 삽입 쿼리를 실행합니다.
        pstmt->executeUpdate();

        // 사용한 자원(준비된 문장, 데이터베이스 연결)을 해제합니다.
        delete pstmt;
        delete con;

        cout << "DB에 데이터를 전송했습니다." << endl; // 성공 메시지 출력
    }
    catch (SQLException& e) {
        // SQL 예외가 발생한 경우 예외 메시지를 출력하고 -1을 반환합니다.
        cout << "SQL 오류: " << e.what() << endl;
        return -1;
    }
    catch (exception& e) {
        // 일반 예외가 발생한 경우 예외 메시지를 출력하고 -1을 반환합니다.
        cout << "오류: " << e.what() << endl;
        return -1;
    }
    return 0; // 성공적으로 데이터 전송 완료
}
