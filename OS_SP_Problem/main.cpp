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
    int used_cubicle = -1 ;
    int used_basket = -1 ;
    int swim_count = 0 ;            //游泳counter，希望能游久一點
    int find_basket_count = 0 ;      //尋找/等待空籃子的counter，太久沒有空籃子的話user會做出反應

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
    Sleep(800);
    user_id++ ;
    if(user_id>=user_thread_num)user_id=0 ;
    if(users[user_id].order_flag==true)check_next=999;
    else if(users[user_id].order_flag!=true)check_next = user_id ;
}

void run()
{
    unique_lock<mutex> lck(mtx);        //上鎖
    ready = true;
    cv.notify_all();                    //release lock
}
void leave_cubicle(int id)
{
    cubicle_arr[users[id].used_cubicle] = -1 ;
    users[id].used_cubicle = -1;

    SetColor(users[id].color,0);
    cout<<users[id].name<<"離開了更衣室，";
    SetColor();

}

void leave_basket(int id)
{
    basket_arr[users[id].used_basket] = -1 ;
    users[id].used_basket = -1;

    SetColor(users[id].color,0);
    cout<<users[id].name<<"清空了他的籃子，";
    SetColor();
}
void go_swimmig(int id)
{
    users[id].state = SWIMMING;
    users[id].swam_flag = true;

    leave_cubicle(id);

    SetColor(users[id].color,0);
    cout<<users[id].name<<"去游泳了\n";
    SetColor();

}

void back_to_waiting_room(int id)
{
    users[id].state = WAITING;
    users[id].swam_flag = false;
    users[id].swim_count = 0 ;
    users[id].find_basket_count = 0;

    //users[id].order_flag = true;
    SetColor(users[id].color,0);
    cout<<users[id].name<<"回去等待室\n";
    SetColor();
}

void find_empty_cubicle(int id)
{
    for(int i =0 ; i<cubicle_num ; i++)
    {
        if(cubicle_arr[i]==-1)//找到空的更衣室了
        {
            cubicle_arr[i]=id;//占用它
            users[id].used_cubicle = i ;
            users[id].state = IN_CUBICLE;//更改user state

            SetColor(users[id].color,0);
            cout<<"第"<<i<<"間更衣室被"<<users[id].name<<"用了\n";
            SetColor();

            return;
        }
    }
    SetColor(users[id].color,0);
    cout<<users[id].name<<"找不到空的更衣室\n";
    SetColor();
}

void find_empty_basket(int id)
{

    for(int i =0 ; i<basket_num ; i++)
    {
        if(basket_arr[i]==-1)//找到空的籃子了
        {
            basket_arr[i]=id;//占用它
            users[id].used_basket = i;

            SetColor(users[id].color,0);

            cout<<users[id].name<<"發現編號"<<i<<"的籃子，內容為"<<basket_arr[i]<<"，是可以用的。因此";
            cout<<users[id].name<<"正在用編號"<<users[id].used_basket<<"的籃子\n";

            SetColor();

            go_swimmig(id);
            return;
        }
    }
    if(users[id].find_basket_count<=1)
    {
        SetColor(users[id].color,0);
        cout<<users[id].name<<"等不到空的籃子，繼續待在更衣室\n";
        SetColor();

        users[id].find_basket_count+=1;
    }else
    {
        SetColor(users[id].color,0);
        cout<<users[id].name<<"等空的籃子等太久了，決定回去等待室\n";
        SetColor();

        leave_cubicle(id);
        back_to_waiting_room(id);
    }

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

        users[id].color=id+2;


        //cout<<id<<" doing something...\n";
        if(users[id].swam_flag==false) //還沒游泳的，準備去游泳
        {
            if(users[id].state==WAITING)
            {
                find_empty_cubicle(id);
                //users[id].order_flag = true;
            }
            else if(users[id].state==IN_CUBICLE)
            {
                find_empty_basket(id);

                //users[id].order_flag = true;
            }

        }

        if(users[id].swam_flag==true)   //游過泳的，準備回等待區
        {
            if(users[id].state==SWIMMING)
            {
                if(users[id].swim_count<=1)
                {
                    users[id].swim_count++;

                    SetColor(users[id].color);
                    cout<<users[id].name<<"還在游泳，";
                    cout<<users[id].name<<"正在用編號"<<users[id].used_basket<<"的籃子\n";
                    SetColor();

                }else{

                    SetColor(users[id].color);
                    cout<<users[id].name<<"游完泳了，";
                    SetColor();

                    find_empty_cubicle(id);

                }
                //users[id].order_flag = true;
            }
            else if(users[id].state==IN_CUBICLE)
            {
                leave_cubicle(id);
                leave_basket(id);
                back_to_waiting_room(id);
                //users[id].order_flag = true;
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
}
