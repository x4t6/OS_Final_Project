#include <iostream>
#include <thread>

using namespace std;

void test_func()
{
  // do something
  double dSum = 0;
  for( int i = 0; i < 10000; ++ i )
    for( int j = 0; j < 10000; ++ j )
      dSum += i*j;

  cout << "Thread: " << dSum << endl;
}

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
