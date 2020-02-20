#include<stdio.h>
#include<string.h>
#include<conio.h>
#include<windows.h>
#define CELL "¡ö"
#define BOARD_SIZE 25
#define DENSITY 0.4
#define SPEED 3
const int maxn=BOARD_SIZE+5;
int dx[]={-1,-1,-1,0,0,1,1,1},dy[]={-1,0,1,-1,1,-1,0,1};
int board[maxn][maxn],newboard[maxn][maxn];
void init_console(HANDLE hout){
  SetConsoleTitle("Game of Life");
  COORD size={BOARD_SIZE*2,BOARD_SIZE};
  SetConsoleScreenBufferSize(hout,size);
  SMALL_RECT rect={0,0,BOARD_SIZE*2-1,BOARD_SIZE-1};
  SetConsoleWindowInfo(hout,1,&rect);
  CONSOLE_CURSOR_INFO cursor_info={1,0};
  SetConsoleCursorInfo(hout,&cursor_info);
}
void print_cell(HANDLE hout,int x,int y,bool live){
  COORD pos={(x-1)*2,y-1};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,live?9:15);
  printf("%s",CELL);
}
void new_board(HANDLE hout,int n){
  for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++){
      board[i][j]=rand()<=RAND_MAX*DENSITY;
      print_cell(hout,i,j,board[i][j]);
  }
}
void update_board(HANDLE hout,int n){
  for(int i=1;i<=n;i++)
    for(int j=1;j<=n;j++){
      int cnt=0;newboard[i][j]=0;
      for(int k=0;k<8;k++)
        cnt+=board[i+dx[k]][j+dy[k]];
      if(cnt==3)newboard[i][j]=1;
      if(cnt==2)newboard[i][j]=board[i][j];
      if(newboard[i][j]!=board[i][j])
        print_cell(hout,i,j,newboard[i][j]);
  }
  memcpy(board,newboard,sizeof(board));
}
int main(){
  HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
  init_console(hout);
  new_board(hout,BOARD_SIZE);
  for(;;){
    update_board(hout,BOARD_SIZE);
    Sleep(1000.0/SPEED);
  }
}
