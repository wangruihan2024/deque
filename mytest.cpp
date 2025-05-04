#include <iostream>
#include "deque.hpp"
using namespace std;
int main() {
    sjtu::deque<int> dq;
    for (int i = 0; i < 4; ++i) {
        dq.push_back(i);
    }
    cout << dq.size() << endl;
    auto it = dq.begin() + dq.size();
    cout << "ok" << endl;
    cout << (it == dq.end()) << endl;
    
    return 0;
}