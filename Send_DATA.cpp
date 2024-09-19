#include "DB_head.h" // ������� �ʿ�� ����
#include<vector>

// �����ͺ��̽��� �ڵ��� ������ �Է��ϴ� �Լ�
int get_INFO(double width, double height, int lensNum, const string& detected_color, const std::vector<uchar>& imageBuffer) {
    try {
        // MySQL ����̹� ��ü�� �����Ͽ� �����ͺ��̽��� �����մϴ�.
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        // ���� �ּ� ~ �̸�, �н����� �����ؾ��� 
        Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL ������ ����
        con->setSchema("open_cv"); // ����� �����ͺ��̽� ��Ű���� ����

        // ����ڷκ��� �ڵ��� ������ �Է� �޽��ϴ�.
        //string name, color;
        //int camera_num, height, width;

        //cout << "�ڵ��� �̸��� �Է��ϼ���: ";
        //cin.ignore(); // ���� �Է¿��� ���� '\n'�� ����
        //getline(cin, name); // �ڵ��� �̸� �Է� �ޱ�

        //// ī�޶� ���� �Է�
        //cout << "ī�޶� ����: ";
        //while (!(cin >> camera_num) || camera_num <= 0) {
        //    cin.clear(); // �Է� ��Ʈ�� ���� �÷��׸� ����
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���� �Է��� ����
        //    cout << "�ٽ� �Է��ϼ���: " << endl;
        //}

        //// �ڵ��� ���� �Է�
        //cout << "����(mm): ";
        //while (!(cin >> height) || height <= 0) {
        //    cin.clear(); // �Է� ��Ʈ�� ���� �÷��׸� ����
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���� �Է��� ����
        //    cout << "�߸��� �Է��Դϴ�. " << endl;
        //}

        //// �ڵ��� �ʺ� �Է�
        //cout << "�ʺ�(mm): ";
        //while (!(cin >> width) || width <= 0) {
        //    cin.clear(); // �Է� ��Ʈ�� ���� �÷��׸� ����
        //    cin.ignore(numeric_limits<streamsize>::max(), '\n'); // ���� �Է��� ����
        //    cout << "�߸��� �Է��Դϴ�. " << endl;
        //}

        //cin.ignore(); // ���� �Է¿��� ���� '\n'�� ����

        //// �ڵ��� ���� �Է�
        //cout << "������ �Է��ϼ���: ";
        //getline(cin, color); // ���� �Է� �ޱ�

        std::stringstream ss;
        ss.write(reinterpret_cast<const char*>(imageBuffer.data()), imageBuffer.size());
        

        // ���� �̸� �ʿ�� ����
        // �ڵ��� ������ �����ͺ��̽��� �����ϱ� ���� SQL �������� �غ��մϴ�.
        // ������ �ʿ�� ��ȯ

        string query = "INSERT INTO phone ( width, height, lensNum, color, image) VALUES (?, ?, ?, ?, ?)";
        PreparedStatement* pstmt = con->prepareStatement(query);
        pstmt->setDouble(1, width); 
        pstmt->setDouble(2, height); 
        pstmt->setInt(3, lensNum); 
        pstmt->setString(4, detected_color);
        pstmt->setBlob(5, &ss); 

        // �����ͺ��̽��� ���� ������ �����մϴ�.
        pstmt->executeUpdate();

        // ����� �ڿ�(�غ�� ����, �����ͺ��̽� ����)�� �����մϴ�.
        delete pstmt;
        delete con;

        cout << "DB�� �����͸� �����߽��ϴ�." << endl; // ���� �޽��� ���
    }
    catch (SQLException& e) {
        // SQL ���ܰ� �߻��� ��� ���� �޽����� ����ϰ� -1�� ��ȯ�մϴ�.
        cout << "SQL ����: " << e.what() << endl;
        return -1;
    }
    catch (exception& e) {
        // �Ϲ� ���ܰ� �߻��� ��� ���� �޽����� ����ϰ� -1�� ��ȯ�մϴ�.
        cout << "����: " << e.what() << endl;
        return -1;
    }
    return 0; // ���������� ������ ���� �Ϸ�
}
