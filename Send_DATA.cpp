#include "DB_head.h" // ������� �ʿ�� ����

// �����ͺ��̽��� �ڵ��� ������ �Է��ϴ� �Լ�
int get_INFO(const string& name, double width, double height, int lensNum, string& detected_color, const vector<uchar>& imageBuffer) {
    try {
        // MySQL ����̹� ��ü�� �����Ͽ� �����ͺ��̽��� �����մϴ�.
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        // ���� �ּ� ~ �̸�, �н����� �����ؾ��� 
        Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL ������ ����
        con->setSchema("open_cv"); // ����� �����ͺ��̽� ��Ű���� ����

        stringstream ss;
        ss.write(reinterpret_cast<const char*>(imageBuffer.data()), imageBuffer.size());

        // ���� �̸� �ʿ�� ����
        // �ڵ��� ������ �����ͺ��̽��� �����ϱ� ���� SQL �������� �غ��մϴ�.
        // ������ �ʿ�� ��ȯ
        string query = "INSERT INTO phone ( name, width, height, lensNum, color, image) VALUES (?, ?, ?, ?, ?, ?)";
        PreparedStatement* pstmt = con->prepareStatement(query);
        pstmt->setString(1, name);
        pstmt->setDouble(2, width);
        pstmt->setDouble(3, height);
        pstmt->setInt(4, lensNum);
        pstmt->setString(5, detected_color);
        pstmt->setBlob(6, &ss);

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
    return 1; // ���������� ������ ���� �Ϸ�
}
