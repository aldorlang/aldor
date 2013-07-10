#include <iostream>
#include "mylist_cc.h"

using namespace std;

int main() {
    MyList<MyString> *ml_str = MyList<MyString>::bracket();
    ml_str = MyList<MyString>::append(MyString::bracket("be there."),ml_str);
    ml_str = MyList<MyString>::append(MyString::bracket("happy to "),ml_str);
    ml_str = MyList<MyString>::append(MyString::bracket(" I am "),ml_str);
    ml_str = MyList<MyString>::append(MyString::bracket("Hello,"),ml_str);

    MyList<MySI> *ml_si = MyList<MySI>::bracket();
    ml_si = MyList<MySI>::append(MySI::bracket(73),ml_si);
    ml_si = MyList<MySI>::append(MySI::bracket(54),ml_si);
    ml_si = MyList<MySI>::append(MySI::bracket(32),ml_si);

    MyString *s = ml_str->total();
    MySI *i = ml_si->total();

    cout << "------------------------------------------" << endl;
    cout << "   Actual C++ program using Aldor types   " << endl;
    cout << "------------------------------------------" << endl;
    cout << "List 1: "; ml_str->output(); cout << endl;
    cout << "List 2: "; ml_si->output(); cout << endl;
    s->output(); cout << endl;
    i->output(); cout << endl;
    cout << "------------------------------------------" << endl;
    cout << "    Call to the Aldor function runlist    " << endl;
    cout << "------------------------------------------" << endl;
    runlist();
}
