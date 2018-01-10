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
bool ready = false;             // �i�D threaduser t 掲各�n�F
int check_next = 0;             // check_next, count , personр�v�Oユ宜�U�@��user

//user�枌�//
int user_thread_num = 0 ;      //Τ�X�咾H
int user_id=0 ;                //�{�b袴圧�鐙�user��id�A�t�Xcheck_next�nノ

//�鵙腴如B�x�l�枌�
int *cubicle_arr;                   //斜�Ocubicle�� ���A�}�C
int *basket_arr;                    //斜�Obasket�� ���A�}�C
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
    bool order_flag = false;        //�o�L���O�困H�Aflag�|怒Θtrue
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
    unique_lock<mutex> lck(mtx);        //�W題
    ready = true;
    cv.notify_all();                    //release lock
}

void find_empty_cubicle(int id){
    for(int i =0 ; i<cubicle_num ; i++){
         if(cubicle_arr[i]==-1){
            cubicle_arr[i]=id;
            users[id].state = IN_CUBICLE;

            cout<<"可"<<i<<"供�鵙腴燃Q"<<users[id].name<<"ノ�F\n";
            system("color �C��");
            return;
         }
    }
    cout<<users[id].name<<"тぃ�讓的婚鵙腴�\n";
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
        if(users[id].swam_flag==false) //匝�S翫�a�此A掲各�h翫�a
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

        if(users[id].swam_flag==true)   //翫�L�a�此A掲各�^汽�欅�
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
    //int id ;                                     //id�O�自畑砂s弦
    cubicle_num = 1;
    basket_num = 2;

    cout<<"出翠�J�X�咾H(殻�h3��)�G\n";
    //cin>>user_thread_num;
    user_thread_num = 3;

    while(user_thread_num<1||user_thread_num>3)
    {
        cout<<"�u�狄蕕J1��3�咾H! :";
        cin>>user_thread_num;
    }
    thread thread_users[user_thread_num];           //�@��thread�N�蹐@��user


    cubicle_arr = new int [cubicle_num];
    basket_arr = new int [basket_num];

    for(int i=0; i<cubicle_num; i++)cubicle_arr[i]=-1;
    for(int i=0; i<basket_num; i++)basket_arr[i]=-1;

    /*** �U�嘘o��for�O�b横ネ user_thread_num �� user ***/
    for (int id = 0; id <user_thread_num; id++)
        thread_users[id] = thread(play, id);

    run();


    for(int id = 0; id <user_thread_num; id++)thread_users[id].join();


    cout << endl;

    return 0;

    return 0;
}
