#include <iostream>

using namespace std;

int main() {
    const int max = 10000;
    for (int i = 1; i <= max; ++i) {
     cout << "\rNow : " << i * 100 / max << "%" << flush;
    }
    cout << endl;
    cout<<"OK";
    return 0;
}
