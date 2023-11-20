#include "vector.hpp"
#include <iostream>

using namespace std;

int main()
{
    Vector<int, LoggingErrorRangeChecker, StdLock> vec = {1, 2, 3};

    cout << "vec: ";
    for (size_t i = 0; i < vec.size(); ++i)
        cout << vec.at(i) << " ";
    cout << endl;

    vec.set_log_file(std::cout);

    vec.at(100);
}