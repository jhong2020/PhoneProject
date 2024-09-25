//#include "DB_head.h" // 헤더파일 필요시 변경
//
//// 사용자로부터 핸드폰 정보를 입력받고, 이를 데이터베이스와 비교하는 함수
//void Input_And_Match_Info(const string& name, int lensNum, int height, int width, const string& detected_color) {
//    try {
//        // 사용자로부터 입력받을 변수 선언
//        //string name, color;
//        //int camera_num, height, width;
//
//        //// 핸드폰 이름을 입력받습니다.
//        //cout << "비교할 핸드폰 이름을 입력하세요: ";
//        //cin.ignore(); // 이전 입력에서 남아 있는 '\n' 문자를 제거
//        //getline(cin, name); // 핸드폰 이름을 한 줄 전체로 입력받음
//
//        //// 카메라 개수를 입력받습니다.
//        //cout << "카메라 개수를 입력하세요: ";
//        //while (!(cin >> camera_num) || camera_num <= 0) { // 입력값이 숫자가 아니거나 0보다 작은 경우 오류 처리
//        //    cin.clear(); // 입력 스트림을 정리합니다.
//        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남아 있는 잘못된 입력을 무시합니다.
//        //    cout << "잘못된 입력입니다. 다시 입력하세요: "; // 잘못된 입력 시 재입력을 요청합니다.
//        //}
//
//        //// 핸드폰 높이를 입력받습니다.
//        //cout << "높이 (mm): ";
//        //while (!(cin >> height) || height <= 0) { // 입력값이 숫자가 아니거나 0보다 작은 경우 오류 처리
//        //    cin.clear(); // 입력 스트림을 정리합니다.
//        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남아 있는 잘못된 입력을 무시합니다.
//        //    cout << "잘못된 입력입니다. 다시 입력하세요: "; // 잘못된 입력 시 재입력을 요청합니다.
//        //}
//
//        //// 핸드폰 너비를 입력받습니다.
//        //cout << "너비 (mm): ";
//        //while (!(cin >> width) || width <= 0) { // 입력값이 숫자가 아니거나 0보다 작은 경우 오류 처리
//        //    cin.clear(); // 입력 스트림을 정리합니다.
//        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // 남아 있는 잘못된 입력을 무시합니다.
//        //    cout << "잘못된 입력입니다. 다시 입력하세요: "; // 잘못된 입력 시 재입력을 요청합니다.
//        //}
//
//        //cin.ignore(); // 이전 입력에서 남아 있는 '\n' 문자를 제거
//
//        //// 핸드폰 색상을 입력받습니다.
//        //cout << "색상을 입력하세요: ";
//        //getline(cin, color); // 사용자가 입력한 핸드폰 색상을 변수 color에 저장합니다.
//
//        // Matching_INFO 함수를 호출하여 데이터베이스와 입력된 정보를 비교합니다.
//        bool result = Matching_INFO(name, lensNum, height, width, detected_color);
//
//        // 비교 결과를 출력합니다.
//        if (result) {
//            cout << "입력한 정보와 데이터베이스의 정보가 일치합니다." << endl;
//        }
//        else {
//            cout << "입력한 정보와 데이터베이스의 정보가 일치하지 않습니다." << endl;
//        }
//    }
//    catch (exception& e) {
//        // 예외 발생 시 오류 메시지를 출력합니다.
//        cout << "오류: " << e.what() << endl;
//    }
//}
