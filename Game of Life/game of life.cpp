#include<stdio.h>
#include<string.h>
#include<time.h>
#include<conio.h>
#include<windows.h>
#define CELL "■"
#define BOARDSIZE 25
#define DENSITY 0.4
#define SPEED 3
const int maxn=BOARDSIZE+5;
int dx[]={-1,-1,-1,0,0,1,1,1},dy[]={-1,0,1,-1,1,-1,0,1};
int board[maxn][maxn],newboard[maxn][maxn];
HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
void init_console(){
  SetConsoleTitle("Game of Life");
  COORD size={BOARDSIZE*2,BOARDSIZE};
  SetConsoleScreenBufferSize(hout,size);
  SMALL_RECT rect={0,0,BOARDSIZE*2-1,BOARDSIZE-1};
  SetConsoleWindowInfo(hout,1,&rect);
  CONSOLE_CURSOR_INFO cursor_info={1,0};
  SetConsoleCursorInfo(hout,&cursor_info);
}
void print_cell(int x,int y,bool live){
  COORD pos={(x-1)*2,y-1};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,live?9:15);
  printf("%s",CELL);
}
void init_board(int n){
  for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++){
      board[i][j]=rand()<=RAND_MAX*DENSITY;
      print_cell(i,j,board[i][j]);
  }
}
void update_board(int n){
  for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++){
      int cnt=0;newboard[i][j]=0;
      for(int k=0;k<8;k++)
        cnt+=board[i+dx[k]][j+dy[k]];
      if(cnt==3)newboard[i][j]=1;
      if(cnt==2)newboard[i][j]=board[i][j];
      if(newboard[i][j]!=board[i][j])
        print_cell(i,j,newboard[i][j]);
  }
  memcpy(board,newboard,sizeof(board));
}
int main(){
  srand(time(0));
  init_console();
  init_board(BOARDSIZE);
  for(;;){
    update_board(BOARDSIZE);
    Sleep(1000.0/SPEED);
  }
}
