#include "DB_head.h" // ������� �ʿ�� ����
#include "Form1.h"
// �־��� �ڵ��� ������ �����ͺ��̽��� �����ϴ��� Ȯ���ϴ� �Լ�
bool Matching_INFO(double width,  double height, int lensNum, const string& detected_color, const std::vector<uchar>& imageBuffer) {
    try {
        // MySQL ����̹� ��ü�� �����Ͽ� �����ͺ��̽��� �����մϴ�.
        mysql::MySQL_Driver* driver = mysql::get_mysql_driver_instance();
        Connection* con = driver->connect("tcp://127.0.0.1:3306", "root", "1234"); // MySQL ������ ���� // DB �ּ�, �̸�, �н����� ����
        con->setSchema("open_cv"); // ����� �����ͺ��̽� ��Ű���� ����

        // Ư�� �ڵ����� ������ ��ȸ�ϱ� ���� SQL �������� �غ��մϴ�.
        // �ʿ�� ������ ����
        string query = "SELECT width, height, lensNum,color,image FROM phone WHERE name = ?"; // 4���� ���� ��ȸ�ϰ�, name���� ���� ��ġ�ϴ� �ุ ��ȸ
        PreparedStatement* pstmt = con->prepareStatement(query);

        //pstmt->setDouble(1, width); // ������ �Ķ���Ϳ� �ڵ��� �̸��� ���ε�

        // ������ �����ϰ� ����� ��ȸ�մϴ�.
        ResultSet* res = pstmt->executeQuery();
        bool match_found = false;

        // ����� �����ϴ��� Ȯ���ϰ�, �Էµ� ������ �����ͺ��̽��� ������ ���մϴ�.
        if (res->next()) {
            double db_width= res->getDouble("width");
            double db_height= res->getDouble("height");
            int db_lensNum = res->getInt("lensNum");
            string db_color = res->getString("color");
            /*std::istream* db_image = res->getBlob("image");*/
            // �Էµ� ������ �����ͺ��̽����� ��ȸ�� ������ ���Ͽ� ��ġ ���θ� Ȯ���մϴ�.
            match_found = (lensNum == db_lensNum) && (height == db_height) &&
                (width == db_width) && (detected_color == db_color);

            // ������ ��ġ�� ��� �����ͺ��̽����� ��ȸ�� ������ ����մϴ�.
            if (match_found) {
                cout << "�Է��� ������ ��ġ�ϴ� DB ������ ã�ҽ��ϴ�:" << endl;
                //cout << "����: " << name << endl;
                cout << "ī�޶� ����: " << db_lensNum << endl;
                cout << "����(mm): " << db_height << endl;
                cout << "�ʺ�(mm): " << db_width << endl;
                cout << "����: " << db_color << endl;

            }
        }

        // ����� �ڿ�(���� ���, �غ�� ����, �����ͺ��̽� ����)�� �����մϴ�.
        delete res;
        delete pstmt;
        delete con;

        // ������ ��ġ�ϸ� true�� ��ȯ, �׷��� ������ false�� ��ȯ�մϴ�.
        return match_found;
    }
    catch (SQLException& e) {
        // SQL ���ܰ� �߻��� ��� ���� �޽����� ����ϰ� false�� ��ȯ�մϴ�.
        cout << "SQL ����: " << e.what() << endl;
        return false;
    }
    catch (exception& e) {
        // �Ϲ� ���ܰ� �߻��� ��� ���� �޽����� ����ϰ� false�� ��ȯ�մϴ�.
        cout << "����: " << e.what() << endl;
        return false;
    }
}
