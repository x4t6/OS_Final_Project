#include <iostream>
#include <thread>

using namespace std;

void OutputValue( int n )
{
  cout << "Number:";
  for( int i = 0; i < n; ++ i )
  {
    this_thread::sleep_for( chrono::duration<int, std::milli>( 5 ) );
    cout << " " << i;
  }
  cout << endl;
}

int main( int argc, char** argv )
{
  cout << "Normal function call" << endl;
  OutputValue( 3 );
  OutputValue( 4 );

  cout << "\nCall function with thread" << endl;
  thread mThread1( OutputValue, 3 );
  thread mThread2( OutputValue, 4 );
  mThread1.join();
  mThread2.join();
  cout << endl;

  return 0;
}
