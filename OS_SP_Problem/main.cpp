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
const int WAITING=0, IN_CUBICLE=1, SWIMMING=2;

//更衣室、籃子相關
int *cubicle_arr;                   //標記cubicle的 狀態陣列
int *basket_arr;                    //標記basket的 狀態陣列
int cubicle_num=1;
int basket_num=2;

const int mapRow=10,mapColumn=25;
const int cleanMapRow=15,cleanMapColumn=60;
bool allRoundDone = false ;
bool space_flag = false;



string theMap[10][25] =
{
    {"┌","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","┐"},

    {"│"," "," "," ","W"," "," "," "," "," ","C"," "," "," "," "," "," "," "," ","S"," "," "," "," ","│"},

    {"│"," "," "," "," "," "," "," "," ","┌","─","┐"," "," "," "," ","┌","─","─","─","┐"," "," "," ","│"},

    {"│"," "," ","["," ","]"," "," "," ","│"," ","│"," "," "," "," ","│"," "," "," ","│"," "," "," ","│"},

    {"│"," "," "," "," "," "," "," "," ","└","─","┘"," "," "," "," ","│"," "," "," ","│"," "," "," ","│"},

    {"│"," "," ","["," ","]"," "," "," "," "," "," "," "," "," "," ","│"," "," "," ","│"," "," "," ","│"},

    {"│"," "," "," "," "," "," "," ","B"," "," "," ","B"," "," "," ","│"," "," "," ","│"," "," "," ","│"},

    {"│"," "," ","["," ","]"," ","["," ","]"," ","["," ","]"," "," ","└","─","─","─","┘"," "," "," ","│"},

    {"│"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ","│"},

    {"└","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","─","┘"},

};

char aCleanMap[15][60] =
{
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
    {' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' ',' '},
};

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
    bool firstRound = true;

    int atMapRow = -1 ;
    int atMapColumn = -1;
    int used_basket_row = -1;
    int used_basket_column = -1;

};

USER users[3];

void SetColor(int f=7,int b=0)
{
    unsigned short ForeColor=f+16*b;
    HANDLE hCon = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hCon,ForeColor);
}

void showMap()
{
    for(int row = 0; row<mapRow; row++)
    {
        for(int column = 0; column<mapColumn; column++)
        {
            for(int id=0; id<3; id++)
            {
                if(row==users[id].atMapRow && column == users[id].atMapColumn)
                {

                    SetColor(users[id].color,0);
                    cout<<users[id].name;
                    SetColor();
                    space_flag = true;
                }
                if(row==users[id].used_basket_row && column == users[id].used_basket_column)
                {

                    SetColor(users[id].color,0);
                    cout<<users[id].name;
                    SetColor();
                    space_flag = true;
                }
            }
            if(space_flag)
            {
                space_flag = false;
            }
            else
            {
                cout<<theMap[row][column];
            }

        }
        cout<<endl;
    }
}

void cleanMap()
{
    for(int row = 0; row<cleanMapRow; row++)
    {
        for(int column = 0; column<cleanMapColumn; column++)
        {
            cout<<aCleanMap[row][column];
        }
        cout<<endl;
    }
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
    user_id++ ;
    if(user_id>=user_thread_num)
    {
        allRoundDone = true;
        showMap();
        Sleep(10000);

        HANDLE hin;
        DWORD WriteWord;
        COORD pos;
        hin = GetStdHandle(STD_OUTPUT_HANDLE);
        pos.X = 0, pos.Y = 0; // 將Caret設在 (5,5) 之地方。

        SetConsoleCursorPosition(hin, pos);
        cleanMap();

        SetConsoleCursorPosition(hin, pos);

        user_id=0 ;

        for(int i =0; i<user_thread_num; i++)
        {
            users[i].order_flag = false;
        }

    }
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

    theMap[users[id].used_basket_row][users[id].used_basket_column]=" ";
    users[id].used_basket_row=-1;
    users[id].used_basket_column=-1;

}
void go_swimmig(int id)
{
    users[id].state = SWIMMING;
    users[id].swam_flag = true;

    leave_cubicle(id);

    SetColor(users[id].color,0);
    cout<<users[id].name<<"去游泳了\n";
    SetColor();
    if(id==0)
    {
        users[id].atMapRow = 3;
        users[id].atMapColumn = 18;
    }
    else if(id==1)
    {
        users[id].atMapRow = 5;
        users[id].atMapColumn = 18;
    }
    else if(id==2)
    {
        users[id].atMapRow = 6;
        users[id].atMapColumn = 18;
    }

}

void back_to_waiting_room(int id)
{
    users[id].state = WAITING;
    users[id].swam_flag = false;
    users[id].swim_count = 0 ;
    users[id].find_basket_count = 0;

    SetColor(users[id].color,0);
    cout<<users[id].name<<"回去等待室\n";
    SetColor();
    if(id==0)
    {
        users[id].atMapRow = 3;
        users[id].atMapColumn = 4;
    }
    else if(id==1)
    {
        users[id].atMapRow = 5;
        users[id].atMapColumn = 4;
    }
    else if(id==2)
    {
        users[id].atMapRow = 7;
        users[id].atMapColumn = 4;
    }
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
            cout<<users[id].name<<"使用了更衣室\n";
            SetColor();
            users[id].atMapRow = 3;
            users[id].atMapColumn = 10;

            return;
        }
    }
    SetColor(users[id].color,0);
    cout<<users[id].name<<"找不到空的更衣室，繼續等待\n";
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

            cout<<users[id].name<<"發現第"<<i+1<<"個籃子是空的，因此";
            cout<<users[id].name<<"使用了第"<<users[id].used_basket+1<<"個籃子\n";

            SetColor();

            if(theMap[7][8]==" ")
            {
                users[id].used_basket_row=7;
                users[id].used_basket_column=8;
                theMap[7][8]=users[id].name;
            }
            else if(theMap[7][12]==" ")
            {
                users[id].used_basket_row=7;
                users[id].used_basket_column=12;
                theMap[7][8]=users[id].name;
            }

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
    }
    else
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
        if(users[id].firstRound)
        {
            users[id].id = id ;
            name();
            //users[id].color=id+2;
            if(id==0)
            {
                users[id].atMapRow = 3 ;
                users[id].atMapColumn = 4;
                users[id].color=10;
                //theMap[users[id].atMapRow][users[id].atMapColumn] = users[id].name;
            }
            else if(id==1)
            {
                users[id].atMapRow = 5 ;
                users[id].atMapColumn = 4;
                users[id].color=12;
                //theMap[users[id].atMapRow][users[id].atMapColumn] = users[id].name;
            }
            else if(id==2)
            {
                users[id].atMapRow = 7 ;
                users[id].atMapColumn = 4;
                users[id].color=11;
                //theMap[users[id].atMapRow][users[id].atMapColumn] = users[id].name;
            }
            users[id].firstRound = false;
            //cout<<"first";
        }
        //cout<<id<<" doing something...\n";
        if(users[id].swam_flag==false) //還沒游泳的，準備去游泳
        {
            if(users[id].state==WAITING)
            {
                find_empty_cubicle(id);
            }
            else if(users[id].state==IN_CUBICLE)
            {
                find_empty_basket(id);
            }
            users[id].order_flag = true;
        }

        if(users[id].swam_flag==true)   //游過泳的，準備回等待區
        {
            if(users[id].state==SWIMMING)
            {
                if(users[id].swim_count<=1)
                {
                    if(users[id].swim_count>0)
                    {
                        SetColor(users[id].color);
                        cout<<users[id].name<<"還在游泳，";
                        cout<<users[id].name<<"正在用編號"<<users[id].used_basket<<"的籃子\n";
                        SetColor();
                    }
                    users[id].swim_count++;
                }
                else
                {
                    SetColor(users[id].color);
                    cout<<users[id].name<<"游完泳了，";
                    SetColor();
                    find_empty_cubicle(id);

                }
            }
            else if(users[id].state==IN_CUBICLE)
            {
                leave_cubicle(id);
                leave_basket(id);
                back_to_waiting_room(id);
            }
            users[id].order_flag = true;
        }

        Key_Process();
        cv.notify_all();
    }//while
}

int main( int argc, char** argv )
{
    //int id ;                                     //id是乘客的編號
    cubicle_num = 1;
    basket_num = 2;

    //cout<<"請輸入幾個人(最多7個)：\n";
    //cin>>user_thread_num;
    user_thread_num = 3  ;

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
