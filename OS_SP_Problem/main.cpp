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

//user�枌�
int user_thread_num = 0 ;                       //Τ�X�咾H
int user_id=0 ;                                 //�{�b袴圧�鐙�user��id�A�t�Xcheck_next�魯�
const int WAITING=0, IN_CUBICLE=1, SWIMMING=2;  //user�昏��A

//�鵙腴如B�x�l�枌�
int *cubicle_arr;                   //斜�Ocubicle�� ���A�}�C
int *basket_arr;                    //斜�Obasket�� ���A�}�C
int cubicle_num=1;
int basket_num=2;

//�a肱�枌�
const int mapRow=10,mapColumn=25;           //�a肱���e
const int cleanMapRow=15,cleanMapColumn=60; //�M�z�e�吋葎墾a肱�昏��e
bool space_flag = false;                    //�O�_�L�Xuser�W�r��flag�A�M�w�nぃ�n�L�泥�


//и�蜜墾a肱
string theMap[10][25] =
{
    {"�z","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�{"},

    {"�x"," "," "," ","W"," "," "," "," "," ","C"," "," "," "," "," "," "," "," ","S"," "," "," "," ","�x"},

    {"�x"," "," "," "," "," "," "," "," ","�z","�w","�{"," "," "," "," ","�z","�w","�w","�w","�{"," "," "," ","�x"},

    {"�x"," "," ","["," ","]"," "," "," ","�x"," ","�x"," "," "," "," ","�x"," "," "," ","�x"," "," "," ","�x"},

    {"�x"," "," "," "," "," "," "," "," ","�|","�w","�}"," "," "," "," ","�x"," "," "," ","�x"," "," "," ","�x"},

    {"�x"," "," ","["," ","]"," "," "," "," "," "," "," "," "," "," ","�x"," "," "," ","�x"," "," "," ","�x"},

    {"�x"," "," "," "," "," "," "," ","B"," "," "," ","B"," "," "," ","�x"," "," "," ","�x"," "," "," ","�x"},

    {"�x"," "," ","["," ","]"," ","["," ","]"," ","["," ","]"," "," ","�|","�w","�w","�w","�}"," "," "," ","�x"},

    {"�x"," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," "," ","�x"},

    {"�|","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�w","�}"},

};

//�M�z�e�吋葎墾a肱
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
    int swim_count = 0 ;            //翫�acounter�A�同罸犂紊[�@�I
    int find_basket_count = 0 ;     //汽�櫑田x�l��counter�Aび�[�SΤ�田x�l�左�user�|圧�Xは棲

    bool order_flag = false;        //�o�L���O�困H�Aflag�|怒Θtrue
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
        }
    }
}

void Key_Process()
{
    user_id++ ;
    if(user_id>=user_thread_num)
    {
        showMap();
        Sleep(10000);

        HANDLE hin;
        DWORD WriteWord;
        COORD pos;
        hin = GetStdHandle(STD_OUTPUT_HANDLE);
        pos.X = 0, pos.Y = 0; // �NCaret�]�b (0,0) ぇ�aよ�C

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
    unique_lock<mutex> lck(mtx);        //�W題
    ready = true;
    cv.notify_all();                    //release lock
}
void leave_cubicle(int id)
{
    cubicle_arr[users[id].used_cubicle] = -1 ;
    users[id].used_cubicle = -1;

    SetColor(users[id].color,0);
    cout<<users[id].name<<"託�}�F�鵙腴如A";
    SetColor();

}

void leave_basket(int id)
{
    basket_arr[users[id].used_basket] = -1 ;
    users[id].used_basket = -1;

    SetColor(users[id].color,0);
    cout<<users[id].name<<"�M�鼎F�L�債x�l�A";
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
    cout<<users[id].name<<"�h翫�a�F\n";
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
    cout<<users[id].name<<"�^�h汽�櫟�\n";
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
        if(cubicle_arr[i]==-1)//т�讓的婚鵙腴任F
        {
            cubicle_arr[i]=id;//�eノウ
            users[id].used_cubicle = i ;
            users[id].state = IN_CUBICLE;//�鵲�user state

            SetColor(users[id].color,0);
            cout<<users[id].name<<"�魯里F�鵙腴�\n";
            SetColor();
            users[id].atMapRow = 3;
            users[id].atMapColumn = 10;

            return;
        }
    }
    SetColor(users[id].color,0);
    cout<<users[id].name<<"тぃ�讓的婚鵙腴如A�~挺汽��\n";
    SetColor();
}

void find_empty_basket(int id)
{

    for(int i =0 ; i<basket_num ; i++)
    {
        if(basket_arr[i]==-1)//т�讓的債x�l�F
        {
            basket_arr[i]=id;//�eノウ
            users[id].used_basket = i;

            SetColor(users[id].color,0);

            cout<<users[id].name<<"�o�{可"<<i+1<<"�喞x�l�O�的此A�]��";
            cout<<users[id].name<<"�魯里F可"<<users[id].used_basket+1<<"�喞x�l\n";

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
        cout<<users[id].name<<"汽ぃ�讓的債x�l�A�~挺�檸b�鵙腴�\n";
        SetColor();

        users[id].find_basket_count+=1;
    }
    else
    {
        SetColor(users[id].color,0);
        cout<<users[id].name<<"汽�的債x�l汽び�[�F�A�M�w�^�h汽�櫟�\n";
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
        if(users[id].swam_flag==false) //匝�S翫�a�此A掲各�h翫�a
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

        if(users[id].swam_flag==true)   //翫�L�a�此A掲各�^汽�欅�
        {
            if(users[id].state==SWIMMING)
            {
                if(users[id].swim_count<=1)
                {
                    if(users[id].swim_count>0)
                    {
                        SetColor(users[id].color);
                        cout<<users[id].name<<"匝�b翫�a�A";
                        cout<<users[id].name<<"タ�bノ�s弦"<<users[id].used_basket<<"�債x�l\n";
                        SetColor();
                    }
                    users[id].swim_count++;
                }
                else
                {
                    SetColor(users[id].color);
                    cout<<users[id].name<<"翫Ч�a�F�A";
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
    //int id ;                                     //id�O�自畑砂s弦
    cubicle_num = 1;
    basket_num = 2;

    //cout<<"出翠�J�X�咾H(殻�h7��)�G\n";
    //cin>>user_thread_num;
    user_thread_num = 3  ;

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
}
