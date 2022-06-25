#include <iostream>
#include "SkipList.h"

int main() {
    {
        struct intLesser {
            bool isLessThan(int a, int b) {
                return a < b;
            }
        };
        SkipList<int, intLesser, int> *mySL = new SkipList<int, intLesser, int>();

        cout << "SKIP LIST DATA : " << endl;
        mySL->insert(4, 88);
        mySL->insert(5, 87);
        mySL->insert(6, 86);
        mySL->insert(2, 82);
        mySL->insert(3, 83);
        mySL->insert(7, 87);
        mySL->insert(10, 90);
        mySL->insert(311, 8311);
        mySL->insert(0, 80);
        mySL->insert(8, 88);
        mySL->insert(9, 89);
        mySL->insert(11, 811);
        mySL->printData();
        //mySL->Search(4);

        int i = 4; // Node to be deleted

        mySL->Delete(i);

        cout << "\n \n \n AFTER DELETION OF : " << i << endl;
        cout << "\n \n \n " << endl;
        mySL->printData();

        cout << endl;
    }

  // try on string
    {
        struct strLesser {
            bool isLessThan(std::string a, std::string b) {
                if (a == Min<std::string>() || b == Max<std::string>())
                    return true;
                if (a == Max<std::string>() || b == Min<std::string>())
                    return false;
                return a < b;
            }
        };

        SkipList<std::string, strLesser, int> *mySL = new SkipList<std::string, strLesser, int>();

        cout << "SKIP LIST DATA : " << endl;
        mySL->insert("a", 100);
        mySL->insert("b", 200);
        mySL->insert("aa", 300);
        mySL->insert("bb", 400);
        mySL->insert("ac", 500);
        mySL->insert("bc", 600);
        mySL->printData();

        auto p = mySL->Search("a");
        cout << "found value: " << p->value << endl;
        p = mySL->Search("bb");
        cout << "found value: " << p->value << endl;
        p = mySL->Search("bc");
        cout << "found value: " << p->value << endl;
        p = mySL->Search("x");
        cout << "found node*: " << p << endl;

        SkipList<std::string, strLesser, int>::Iterator p0 = mySL->lower_bound("ac");
        SkipList<std::string, strLesser, int>::Iterator p1 = mySL->upper_bound("bx");
        while (p0 != p1) {
            cout << "(" << p0->key <<":" << p0->value << ") ";
            ++p0;
        }
        cout << endl;
    }

    return 0;
}