#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

mutex mtx;                      // critical section�困�ジ題
condition_variable cv;          // がジ題��condition variable
bool ready = false;             // �i�D threadperson t 掲各�n�F
int check_next = 0;             // check_next, count , personр�v�Oユ宜�U�@��person

class USER
{
private:

public:
    int atfloor;
    int tofloor;
    int dir=0;              //よ�V
    int order_flag=0;       //�o�L���O�困H�Aflag�|怒Θ1
    int inelevator_flag=0;   //�H�b�b�q沿弧��1
    int id=0;
    char name;
    int live=0;             //��1�宛�user�|�X�{
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
