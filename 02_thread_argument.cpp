#include <iostream>
#include <thread>

using namespace std;

void hello(string name)
{
    cout << name << endl;
}

int main()
{

    thread t(hello, "hi dude");
    t.join();

    return 0;
}