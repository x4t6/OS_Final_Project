#include <iostream>
#include <thread>
#include <mutex>
#include <condition_variable>
#include <cstdlib>
#include <ctime>
#include <windows.h>

using namespace std;

mutex mtx;                      // critical section��������
condition_variable cv;          // �����ꪺcondition variable
bool ready = false;             // �i�D threadperson t �ǳƦn�F
int check_next = 0;             // check_next, count , person���v�O�浹�U�@��person

class USER
{
private:

public:
    int atfloor;
    int tofloor;
    int dir=0;              //��V
    int order_flag=0;       //�o�L���O���H�Aflag�|�ܦ�1
    int inelevator_flag=0;   //�H�b�b�q��̬�1
    int id=0;
    char name;
    int live=0;             //��1����user�|�X�{
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
