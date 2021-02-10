// 在任何时刻,Esc键退出,Backspace键重新开始
// 在游戏过程中,空格键暂停/继续,W/S键选择卡槽中的植物,方向键选择草地,Enter键种植物,-键铲掉植物
// 在给卡槽放卡片时,空格键开始游戏,W/S键选择卡槽中的卡片,方向键选择其它卡片,Enter键交换两张卡片.若所有的卡片都装不满卡槽,则没有最后两个操作
#include<stdio.h>
#include<math.h>
#include<time.h>
#include<conio.h>
#include<windows.h>
// 整个游戏每隔UPDINT个"时间"更新一次.可以看成单位时间里有UPDINT个单位"时间"
// 代码中会出现很多类似于ready()和turn()的函数.在注释中,前者被描述为"是否准备更新",后者为"此时是否更新".实际表达的含义是,上一次更新的时间为prev,根据更新的间隔inter,这一次更新的时间就应该为prev+inter,仅在此时turn()=true.然而某些情况下此时并不一定就要更新,而是需要满足一定的条件,那么prev+inter之后的时间里ready()一直为true,直到条件满足,受到更新为止
// 实际上,由于更新的时间一定是UPDINT的倍数,可能不存在prev+inter这个时间,因此代码中对这两个函数做了一些调整 
#define UPDINT 100
// linkes_list: 手写链表 
template<typename Tp>
class linked_list{
public:
  struct node{
    Tp val;bool exi;node *next;
  };
  int tot,poltop;
  node nodes[1<<20],*pool[1<<20],*now;
  void insert(Tp t){
    node *p=poltop?pool[poltop--]:nodes+(++tot);
    p->val=t,p->exi=1,p->next=now,now=p;
  }
  void remove(node *p,node *prev){
    if(now==p)now=p->next;
    if(prev)prev->next=p->next;
    pool[++poltop]=p,p->exi=0;
  }
};
// hout: 标准输出句柄
// flgcnt: 记录已完成的轮数.每一轮开始之前有一次重新给卡槽放卡片的机会
// wavcnt: 记录当前这一轮已完成的波数.每一轮有20波僵尸。
// timcnt: 记录从游戏开始到现在的"时间",期望的单位是ms.
// suncnt: 目前阳光的数量
// chid,chx,chy: W/S键和方向键选择的位置坐标,卡槽只有一列,只需要一个坐标
HANDLE hout=GetStdHandle(STD_OUTPUT_HANDLE);
int flgcnt,wavcnt,timcnt,suncnt;
int chid,chx,chy;
// sun_gener: 阳光生成 
struct sun_gener{
  // prod<uct>: 每次生成的阳光数量
  // inter<val>: 生成阳光的间隔
  // prev<{ious>: 上一次生成阳光的时间
  int prod,inter,prev;
  // turn(): 此时是否生成阳光
  // print(): 打印阳光的数量
  // update(): 生成阳光
  bool turn(){
    return prev+inter<=timcnt&&prev+inter+UPDINT>timcnt;
  }
  void print();
  void update();
};
sun_gener sunG;
sun_gener sunGlib[]={
#include"sunGlib.data"
};
// zombie_gener: 僵尸生成
struct zombie_gener{
  // prod<uct>[x]:第一轮第x波僵尸的数量.从第二轮开始,每波僵尸的数量均为prod[19]
  // range[x]: 第一轮第x波僵尸的种类数量,即该波生成种类编号为[0,range[x])的僵尸.从第二轮开始,每波僵尸的种类数量比上一轮该波多5
  // inter<val>: 生成僵尸的间隔.特别地,除第一轮第一波外,若草地上已经没有僵尸,会立即新生成一波僵尸
  // prev<ious>: 上一次生成僵尸的时间
  int prod[20],range[20],inter,prev;
  // turn(): 此时是否生成僵尸(不包括草地上没有僵尸的情况)
  // print(): 打印当前的轮数
  // update(): 生成僵尸
  bool turn(){
    return prev+inter<=timcnt&&prev+inter+UPDINT>timcnt;
  }
  void print();
  void update();
};
zombie_gener zmbG;
zombie_gener zmbGlib[]={
#include"zmbGlib.data"
};
// card: 卡片 
struct card{
  // type: 对应植物的字符
  // typid: 对应植物的种类编号
  // attr<ibute>: 对应植物的文本属性
  // cooldown: 冷却时间
  // cost: 花费的阳光数量
  // prev<ious>: 上一次使用的时间
  char type;int typid,attr;
  int cooldown,cost,prev;
  bool operator<(card rhs)const{return typid<rhs.typid;}
  // ready():是否可以使用。
  // print(x,y,chosen): 在第x列y行打印卡片.chosen标记是否被选中
  bool ready(){
    return prev+cooldown<=timcnt&&cost<=suncnt;
  }
  void print(int x,int y,bool chosen);
};
card cards[5][6];
card cardlib[]={
#include"cardlib.data"
};
// 草地&植物
// 每种植物分为两个状态,分别对应ready()为true和false的情况.两个不同的状态可以辨别某些植物(尤其是trig=true的植物)是否在生产/攻击的间隔中,例如地雷埋在土里和冒出地面就是两个不同的状态
struct lawn{
  // type[x]: 两种状态对应的字符
  // typid: 种类编号
  // attr<ibute>[x]: 两种状态对应的文本属性
  // attack: 攻击力.这里特指炸弹类植物的伤害,射击类植物的伤害在子弹中体现
  // slow: 提供的减速时间
  // freeze: 提供的冻结时间
  // range: 攻击范围.这里特指炸弹类植物放射状的攻击范围
  // bul<let>range: 射程
  // prod<uct>: 阳光产量
  // hit: 生命值
  // inter<val>: 生产/攻击的间隔
  // prev<ious>: 上一次生产/攻击的时间
  // dispos<able>: 是否为一次性植物.用于处理炸弹类植物
  // single: 是否一次只能伤害一个僵尸.用于处理大嘴花
  // trig<ger>: 是否由僵尸触发.用于处理地雷,大嘴花
  // obstr<uct>: 是否能阻挡僵尸越过.用于处理高坚果
  char type[2];int typid,attr[2];
  int attack,slow,freeze,range,bulrange,prod,hit,inter,prev;
  bool dispos,single,trig,obstr;
  bool operator<(lawn rhs)const{return typid<rhs.typid;}
  // ready(): 是否准备生产/攻击.用于处理由僵尸触发的植物
  // turn(): 此时是否生产/攻击
  // print(x,y,chosen): 在草地的第x列y行打印草地,同时打印在这块草地上的植物,子弹,僵尸.若三者各自有重叠,优先打印种类编号较大的;若三者之间有重叠,优先打印植物,其次打印僵尸.chosen标记是否被选中
  // update(x,y): 生产阳光;对攻击范围内的僵尸进行攻击,减速,冻结;若该植物有子弹,在该位置生成子弹
  bool ready(){return prev+inter<=timcnt;}
  bool turn(){
    return prev+inter<=timcnt&&prev+inter+UPDINT>timcnt;
  }
  void print(int x,int y,bool chosen);
  void update(int x,int y);
};
lawn lawnmap[9][5];
lawn emptlawn={{0,0},-1};
lawn lawnlib[]={
#include"lawnlib.data"
};
// bullet: 子弹 
struct bullet{
  // type：字符
  // typid：对应植物的种类编号
  // x,y：列数,行数.这里的行列数与草地略有区别,因为一块草地是由两个字符拼起来的,而一个子弹只占一个字符
  // attr：文本属性
  // attack：攻击力
  // slow：提供的减速时间
  // freeze：提供的冻结时间
  // dest：能到达的最远位置.由原植物的位置和射程决定
  // inter：移动的间隔
  // prev：上一次移动的时间
  // single：是否只能伤害一个僵尸.用于处理大喷菇
  char type;int typid,x,y,attr;
  int attack,slow,freeze,dest,inter,prev;
  bool single;
  bool operator<(bullet rhs)const{return typid<rhs.typid;}
  // turn()：此时是否移动
  // update()：移动;对遇到的僵尸进行攻击,减速,冻结
  bool turn(){
    return prev+inter<=timcnt&&prev+inter+UPDINT>timcnt;
  }
  bool update();
};
typedef linked_list<bullet>::node bulnode;
linked_list<bullet> bullets;
bullet nobul={0,-1};
bullet bullib[]={
#include"bullib.data"
};
// zombie: 僵尸
// 每个僵尸分为三个状态,分别对应下文skip=0且hit>point,skip=0且hit<=point,skip>0的情况.对skip的区分可以实现撑杆僵尸跳跃前后不同的移动间隔;对hit的区分可以实现路障铁桶变成普僵等变化
struct zombie{
  // type[x]：三种状态对应的字符
  // typid：种类编号
  // x,y：列数,行数.这里的行列数与子弹相同
  // attr<ibute>[x]：三种状态对应的文本属性
  // attack：攻击力
  // slow：减速效果结束的时间
  // freeze：冻结效果结束的时间.冻结效果会覆盖减速效果
  // skip：最多能跳过的植物数量.用于处理撑杆僵尸
  // hit：生命值
  // point：前两种状态的临界生命值
  // w<alk>inter<val>[x]：三种状态对应的移动间隔
  // b<ite>inter<val>：攻击间隔
  // prev<ious>：上一次移动/攻击的时间.由于移动和攻击不可能同时进行,只记一个prev就够了
  // proof[x]：三种状态下是否防炸弹的减速和冻结效果
  // bul<let>proof[x]：三种状态下是否防子弹的减速和冻结效果
  char type[3];int typid,x,y,attr[3];
  int attack,slow,freeze,skip,hit,point;
  int winter[3],binter,prev;
  bool proof[3],bulproof[3];
  bool operator<(zombie rhs)const{return typid<rhs.typid;}
  // state()：此时的状态
  // w<alk>ready()：是否准备移动
  // b<ite>ready()：是否准备攻击
  // s<low>turn()：此时是否消除减速效果.减速效果需要修改移动和攻击间隔,因此需要一个函数
  // update()：移动,攻击
  int state(){return skip?2:hit<=point;}
  bool wready(){return prev+winter[state()]<=timcnt;}
  bool bready(){return prev+binter<=timcnt;}
  bool sturn(){
    return timcnt<slow&&timcnt+UPDINT>=slow;
  }
  void update();
};
typedef linked_list<zombie>::node zmbnode;
linked_list<zombie> zombies;
zombie nozmb={{0,0,0},-1};
zombie zmblib[]={
#include"zmblib.data"
};

int crdtot=sizeof(cardlib)/sizeof(card);
int zmbtot=sizeof(zmblib)/sizeof(zombie);

void sun_gener::print(){
  COORD pos={5,0};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,78);
  printf("*");
  SetConsoleTextAttribute(hout,79);
  printf("%4d",suncnt<10000?suncnt:9999);
}
void sun_gener::update(){
  if(turn())prev=timcnt,suncnt+=prod;
}
void zombie_gener::print(){
  COORD pos={10,0};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,110);
  printf("     flag: %d",flgcnt);
}
void zombie_gener::update(){
  if(turn()||!zombies.now&&(flgcnt||wavcnt)){
    int cprod=prod[flgcnt?19:wavcnt];
    int cr=flgcnt*5+range[wavcnt];
    if(cr>zmbtot)cr=zmbtot;
    prev=timcnt,wavcnt++;
    for(int i=0;i<cprod;i++){
      zombie zmb=zmblib[rand()%cr];
      zmb.y=rand()%5,zmb.prev=timcnt;
      zombies.insert(zmb);
    }
  }
}
void card::print(int x,int y,bool chosen){
  COORD pos={x*5,y};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,ready()?attr|8:attr);
  printf("%c",type);
  SetConsoleTextAttribute(hout,chosen?176:0);
  printf(" ");
  SetConsoleTextAttribute(hout,ready()?240:112);
  printf("%3d",cost);
}
void lawn::print(int x,int y,bool chosen){
  COORD pos={x*2+6,y+1};
  SetConsoleCursorPosition(hout,pos);
  bullet bul1=nobul,bul2=nobul;
  for(bulnode *p=bullets.now;p;p=p->next){
    bullet &cbul=p->val;
    if(cbul.x==x*2&&cbul.y==y&&bul1<cbul)bul1=cbul;
    if(cbul.x==x*2+1&&cbul.y==y&&bul2<cbul)bul2=cbul;
  }
  zombie zmb1=nozmb,zmb2=nozmb;
  for(zmbnode *p=zombies.now;p;p=p->next){
    zombie &czmb=p->val;
    if(czmb.x==x*2&&czmb.y==y&&zmb1<czmb)zmb1=czmb;
    if(czmb.x==x*2+1&&czmb.y==y&&zmb2<czmb)zmb2=czmb;
  }
  int cattr;char c;
  int st=!ready(),zmbst=zmb1.state();
  if(~typid)cattr=attr[st]|32,c=type[st];
  else if(~zmb1.typid)
    cattr=zmb1.attr[zmbst]|32,c=zmb1.type[zmbst];
  else if(~bul1.typid)
    cattr=bul1.attr|32,c=bul1.type;
  else cattr=32,c=' ';
  SetConsoleTextAttribute(hout,cattr);
  printf("%c",c);
  int bkg=chosen?176:32;zmbst=zmb2.state();
  if(~zmb2.typid)
    cattr=zmb2.attr[zmbst]|bkg,c=zmb2.type[zmbst];
  else if(~bul2.typid)
    cattr=bul2.attr|bkg,c=bul2.type;
  else cattr=bkg,c=' ';
  SetConsoleTextAttribute(hout,cattr);
  printf("%c",c);
}
void lawn::update(int x,int y){
  if(!trig&&!turn()||trig&&!ready())return;
  bool f=!trig;
  for(zmbnode *p=zombies.now;p;p=p->next){
    zombie &czmb=p->val;
    if(czmb.x>=x*2&&czmb.x<=x*2+1&&czmb.y==y&&!czmb.skip){f=1;break;}
  }
  if(!f)return;
  prev=timcnt,suncnt+=prod;
  for(zmbnode *p=zombies.now,*q=0;p;p=p->next){
    zombie &czmb=p->val;
    if(abs(czmb.x-x*2)<=range*2+1&&abs(czmb.y-y)<=range){
      czmb.hit-=attack;
      if(slow&&!czmb.proof[czmb.state()]){
        if(czmb.slow<timcnt){
          for(int k=0;k<3;k++)czmb.winter[k]*=2;
          czmb.binter*=2;
        }
        czmb.slow=timcnt+slow;
      }
      if(freeze&&!czmb.proof[czmb.state()])
        czmb.freeze=timcnt+freeze;
      if(czmb.hit<=0)zombies.remove(p,q);
      if(single)break;
    }
    if(p->exi)q=p;
  }
  bullet bul=bullib[typid];
  if(bul.type)for(zmbnode *p=zombies.now;p;p=p->next){
    zombie &czmb=p->val;
    if(czmb.x>=x*2&&czmb.x<=(x+bulrange)*2+1&&czmb.y==y){
      bul.x=x*2,bul.y=y,bul.dest=(x+bulrange)*2+1;
      bul.prev=timcnt;
      bullets.insert(bul);break;
    }
  }
  if(dispos)*this=emptlawn;
}
bool bullet::update(){
  if(turn())x++,prev=timcnt;
  if(x>17||x>dest)return 1;
  for(zmbnode *p=zombies.now,*q=0;p;p=p->next){
    zombie &czmb=p->val;
    if(x==czmb.x&&y==czmb.y){
      czmb.hit-=attack;
      if(slow&&!czmb.bulproof[czmb.state()]){
        if(czmb.slow<timcnt){
          for(int k=0;k<3;k++)czmb.winter[k]*=2;
          czmb.binter*=2;
        }
        czmb.slow=timcnt+slow;
      }
      if(freeze&&!czmb.bulproof[czmb.state()])
        czmb.freeze=timcnt+freeze;
      if(czmb.hit<=0)zombies.remove(p,q);
      if(single)return 1;
    }
    if(p->exi)q=p;
  }
}
void zombie::update(){
  lawn &pos=lawnmap[x/2][y];
  if(pos.obstr)skip=0;
  if(freeze<timcnt){
    if(!skip){
      if(~pos.typid&&bready()){
        pos.hit-=attack,prev=timcnt;
        if(pos.hit<=0)pos=emptlawn;
      }
      if(!~pos.typid&&wready())x--,prev=timcnt;
    }
    else if(wready()){
      x--,prev=timcnt;
      if(~pos.typid)skip--,x-=!(x&1);
    }
  }
  if(sturn()){
    for(int k=0;k<3;k++)winter[k]/=2;
    binter/=2;
  }
}

void print_background(){
  COORD pos;
  for(int i=0;i<6;i++){
    pos.X=5,pos.Y=i;
    SetConsoleCursorPosition(hout,pos);
    SetConsoleTextAttribute(hout,96);
    printf("                    ");
  }
  pos.X=11,pos.Y=0;
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,72);
  printf("+");
  SetConsoleTextAttribute(hout,76);
  printf("-");
}
void init_console(){
  SetConsoleTitle("char pvz");
  COORD size={25,6};
  SetConsoleScreenBufferSize(hout,size);
  SMALL_RECT rect={0,0,24,5};
  SetConsoleWindowInfo(hout,1,&rect);
  CONSOLE_CURSOR_INFO cursor_info={1,0};
  SetConsoleCursorInfo(hout,&cursor_info);
}
void init_game(){
  sunG=sunGlib[0],zmbG=zmbGlib[0],suncnt=50;
  bullets.tot=bullets.poltop=0,bullets.now=0;
  zombies.tot=zombies.poltop=0,zombies.now=0;
  for(int i=0;i<5;i++)
    for(int j=0;j<6;j++)
      if(i*6+j<crdtot)cards[i][j]=cardlib[i*6+j];
  for(int i=0;i<9;i++)
    for(int j=0;j<5;j++)lawnmap[i][j]=emptlawn;
}
int stop_game(){
  COORD pos={7,2};
  SetConsoleCursorPosition(hout,pos);
  SetConsoleTextAttribute(hout,15);
  printf("GAME PAUSED");
  for(;;)if(kbhit())switch(getch()){
  case 27:return 0;
  case 32:return 1;
  }
}
int judge_game(){
  for(zmbnode *p=zombies.now;p;p=p->next){
    zombie &czmb=p->val;
    if(czmb.x>=0)continue;
    COORD pos={czmb.x+6,czmb.y+1};
    SetConsoleCursorPosition(hout,pos);
    int cattr=czmb.attr[czmb.state()]|96;
    SetConsoleTextAttribute(hout,cattr);
    printf("%c",czmb.type[czmb.state()]);
    pos.X=1,pos.Y=2;
    SetConsoleCursorPosition(hout,pos);
    SetConsoleTextAttribute(hout,10);
    printf("ZOMBIES ATE YOUR BRAIN!");
    for(;;)if(kbhit())switch(getch()){
    case 27:return 0;
    case 8:return 2;
    }
  }
  return 1;
}
int update_card(){
  if(kbhit())switch(getch()){
  case 27:return 0;
  case 8:return 2;
  case 32:return 3;
  case 119:if(chid)chid--;break;
  case 115:if(chid<5&&chid+1<crdtot)chid++;break;
  case 224:
    switch(getch()){
    case 72:if(chy)chy--;break;
    case 80:if(chy<5&&chx*6+chy+1<crdtot)chy++;break;
    case 75:if(chx>1)chx--;break;
    case 77:if(chx<4&&chx*6+chy+6<crdtot)chx++;break;
    }
    break;
  case 13:
    if(chx*6+chy<crdtot){
      card tmp=cards[0][chid];
      cards[0][chid]=cards[chx][chy],cards[chx][chy]=tmp;
    }
    break;
  }
  for(int i=0;i<5;i++)
    for(int j=0;j<6;j++)if(i*6+j<crdtot){
      bool chosen=!i&&chid==j||chx==i&&chy==j;
      cards[i][j].print(i,j,chosen);
  }
  return 1;
}
int update_game(){
  if(kbhit())switch(getch()){
  case 27:return 0;
  case 8:return 2;
  case 32:if(!stop_game())return 0;break;
  case 119:if(chid)chid--;break;
  case 115:if(chid<5&&chid+1<crdtot)chid++;break;
  case 224:
    switch(getch()){
    case 72:if(chy)chy--;break;
    case 80:if(chy<4)chy++;break;
    case 75:if(chx)chx--;break;
    case 77:if(chx<8)chx++;break;
    }
    break;
  case 13:
    if(cards[0][chid].ready()&&!~lawnmap[chx][chy].typid){
      int cid=cards[0][chid].typid;
      lawnmap[chx][chy]=lawnlib[cid];
      cards[0][chid].prev=timcnt;
      lawnmap[chx][chy].prev=timcnt+lawnlib[cid].prev;
      suncnt-=cards[0][chid].cost;
    }
    break;
  case 45:lawnmap[chx][chy]=emptlawn;break;
  }
  sunG.update();
  if(wavcnt<19)zmbG.update();
  else if(!zombies.now)return 3;
  for(int i=0;i<9;i++)
    for(int j=0;j<5;j++)lawnmap[i][j].update(i,j);
  for(bulnode *p=bullets.now,*q=0;p;p=p->next){
    bullet &cbul=p->val;
    if(cbul.update())bullets.remove(p,q);
    if(p->exi)q=p;
  }
  for(zmbnode *p=zombies.now,*q=0;p;p=p->next){
    zombie &czmb=p->val;
    czmb.update();
    if(p->exi)q=p;
  }
  sunG.print();
  zmbG.print();
  for(int i=0;i<6&&i<crdtot;i++)
    cards[0][i].print(0,i,chid==i);
  for(int i=0;i<9;i++)
    for(int j=0;j<5;j++)
      lawnmap[i][j].print(i,j,chx==i&&chy==j);
  Sleep(UPDINT);timcnt+=UPDINT;
  return judge_game();
}
// ret=0退出游戏;ret=1什么也不做;ret=2重新开始;ret=3结束当前环节(视情况而定)
int choose_card(){
  print_background();
  sunG.print();
  zmbG.print();
  for(int i=0;i<9;i++)
    for(int j=0;j<5;j++)lawnmap[i][j].print(i,j,0);
  for(;;){
    int ret=update_card();
    if(ret==3)return 1;
    if(ret!=1)return ret;
  }
}
int start_game(){
  print_background();
  wavcnt=0;
  for(;;){
    int ret=update_game();
    if(ret==3)return 1;
    if(ret!=1)return ret;
  }
}
int run_game(){
  int ret;timcnt=UPDINT;
  for(flgcnt=0;;flgcnt++){
    chid=chy=0,chx=1,ret=choose_card();
    if(ret!=1)return ret;
    chid=chx=chy=0,ret=start_game();
    if(ret!=1)return ret;
  }
}
int main(){
  srand(time(0));
  init_console();
  do init_game();while(run_game());
  return 0;
}
