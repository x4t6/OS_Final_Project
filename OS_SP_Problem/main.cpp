#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

mutex mtx;                      // critical section的互斥鎖
condition_variable cv;          // 互斥鎖的condition variable
bool ready = false;             // 告訴 threadperson t 準備好了
int check_next = 0;             // check_next, count , person把權力交給下一個person

class USER
{
private:

public:
    int atfloor;
    int tofloor;
    int dir=0;              //方向
    int order_flag=0;       //發過指令的人，flag會變成1
    int inelevator_flag=0;   //人在在電梯裡為1
    int id=0;
    char name;
    int live=0;             //為1為該user會出現
    int color;
};


int main( int argc, char** argv )
{
  // execute thread
  thread mThread( test_func );

  // do somthing
  cout << "main thread" << endl;

  // wait the thread stop
  mThread.join();

  return 0;
}
