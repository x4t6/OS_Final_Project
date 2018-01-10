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
bool ready = false;             // 告訴 threaduser t 準備好了
int check_next = 0;             // check_next, count , person把權力交給下一個user

//user相關//
int user_thread_num = 0 ;      //有幾個人
int user_id=0 ;                //現在該做事的user的id，配合check_next要用

//更衣室、籃子相關
int *cubicle_arr;                   //標記cubicle的 狀態陣列
int *basket_arr;                    //標記basket的 狀態陣列
int cubicle_num=1;
int basket_num=2;

const int WAITING=0, IN_CUBICLE=1, SWIMMING=2;

class USER
{
private:

public:
    char name;
    int id=0;
    int color;


    int state=0;
    bool order_flag = false;        //發過指令的人，flag會變成true
    bool swam_flag = false;

};

USER users[3];

void SetColor(int f=7,int b=0)
{
    unsigned short ForeColor=f+16*b;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon,ForeColor);
}

void name()
{
    for(int i=0 ; i<user_thread_num; i++)
    {
        switch(users[i].id)
        {
        case 0:
            users[i].name='A';
            break;
        case 1:
            users[i].name='B';
            break;
        case 2:
            users[i].name='C';
            break;
        case 3:
            users[i].name='D';
            break;
        case 4:
            users[i].name='E';
            break;
        case 5:
            users[i].name='F';
            break;
        case 6:
            users[i].name='G';
            break;
        case 7:
            users[i].name='H';
            break;
        case 8:
            users[i].name='J';
            break;
        case 9:
            users[i].name='K';
            break;
        case 10:
            users[i].name='L';
            break;
        case 11:
            users[i].name='M';
            break;
        case 12:
            users[i].name='N';
            break;
        case 13:
            users[i].name='P';
            break;
        case 14:
            users[i].name='Q';
            break;

        }
    }
}

void Key_Process()
{
    Sleep(200);
    user_id++ ;
    if(user_id>=user_thread_num)user_id=0 ;
    if(users[user_id].order_flag==1)check_next=999;
    else if(users[user_id].order_flag!=1)check_next = user_id ;
}

void run()
{
    unique_lock<mutex> lck(mtx);        //上鎖
    ready = true;
    cv.notify_all();                    //release lock
}

void find_empty_cubicle(int id){
    for(int i =0 ; i<cubicle_num ; i++){
         if(cubicle_arr[i]==-1){
            cubicle_arr[i]=id;
            users[id].state = IN_CUBICLE;

            cout<<"第"<<i<<"間更衣室被"<<users[id].name<<"用了\n";
            system("color 顏色");
            return;
         }
    }
    cout<<users[id].name<<"找不到空的更衣室\n";
    SetColor();
}

void play(int id)
{

    while(true)
    {
        unique_lock<mutex> lck(mtx);
        while(id != check_next || !ready  )
        {
            cv.wait(lck);
        }
        check_next=999;

        users[id].id = id ;
        name();
        users[id].color=id+1;

        srand(time(NULL));
        Sleep(200);

        cout<<id<<" doing something...\n";
        if(users[id].swam_flag==false) //還沒游泳的，準備去游泳
        {
            if(users[id].state==WAITING)
            {
                find_empty_cubicle(id);

            }
            else if(users[id].state==IN_CUBICLE)
            {

            }
            else if(users[id].state==SWIMMING)
            {

            }
        }

        if(users[id].swam_flag==true)   //游過泳的，準備回等待區
        {
            if(users[id].state==WAITING)
            {

            }
            else if(users[id].state==IN_CUBICLE)
            {

            }
            else if(users[id].state==SWIMMING)
            {

            }
        }

        Key_Process();
        cv.notify_all();
    }

}

int main( int argc, char** argv )
{
    //int id ;                                     //id是乘客的編號
    cubicle_num = 1;
    basket_num = 2;

    cout<<"請輸入幾個人(最多3個)：\n";
    //cin>>user_thread_num;
    user_thread_num = 3;

    while(user_thread_num<1||user_thread_num>3)
    {
        cout<<"只能輸入1到3個人! :";
        cin>>user_thread_num;
    }
    thread thread_users[user_thread_num];           //一個thread代表一個user


    cubicle_arr = new int [cubicle_num];
    basket_arr = new int [basket_num];

    for(int i=0; i<cubicle_num; i++)cubicle_arr[i]=-1;
    for(int i=0; i<basket_num; i++)basket_arr[i]=-1;

    /*** 下面這個for是在產生 user_thread_num 個 user ***/
    for (int id = 0; id <user_thread_num; id++)
        thread_users[id] = thread(play, id);

    run();


    for(int id = 0; id <user_thread_num; id++)thread_users[id].join();


    cout << endl;

    return 0;

    return 0;
}
