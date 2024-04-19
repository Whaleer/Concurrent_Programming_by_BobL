#include <iostream>
#include <thread>

using namespace std;

void hello()
{
    cout << "hello from new thread" << endl;
}

int main()
{
    thread t(hello);
    t.join();

    thread t2([]
              { cout << "hello from lambda thread." << endl; });

    t2.join();

    return 0;
}