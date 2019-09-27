/*
input:
  1 2 9
  4 6 3
  7 8 5
output:
  Step #1: 3 9
  Step #2: 5 6
  Step #3: 6 9
*/
#include<cstdio>
#include<cmath>
#include<cstring>
#include<cctype>
#include<algorithm>
#include<queue>
using namespace std;
const int maxn=362880+5;

int fact[]={1,1,2,6,24,120,720,5040,40320,362880};
struct state{
  int st[10];
  state operator=(int arr[10]){
    memcpy(st,arr,10);return *this;
  }
  int &operator[](int x){return st[x];}
};
int encode(state st){
  int cnt=(st[1]-1)*fact[8];
  for(int i=2;i<=9;i++){
    int tmp=0;
    for(int j=1;j<i;j++)tmp+=st[j]<st[i];
    cnt+=(st[i]-tmp-1)*fact[9-i];
  }
  return cnt+1;
}
state decode(int c){
  state ans;int vis=0;c--;
  for(int i=1;i<=9;i++){
    int j,t=c/fact[9-i];
    for(j=1;j<=9;j++)if(!(vis>>j&1)&&!t--)break;
    ans[i]=j,vis|=1<<j,c%=fact[9-i];
  }
  return ans;
}

int dx1[]={-1,1,0,0},dy1[]={0,0,-1,1};
int dx2[]={-1,-1,1,1},dy2[]={-1,1,-1,1};
class solver{
public:
  bool vis[maxn];
  int fa[maxn],step[maxn],type[maxn];
  pair<int,int> opt[maxn];
  void bfs(state st,int type_st){
    memset(vis,0,sizeof(vis));
    int u=encode(st),v;
    state cur,next;
    vis[u]=1,fa[u]=-1,step[u]=0;
    type[u]=type_st^1;
    queue<int> q;q.push(u);
    while(!q.empty()){
      u=q.front();q.pop();
      if(u==1)return;
      cur=decode(u);
      for(int x=1;x<=3;x++){
        for(int y=1;y<=3;y++){
          int pos=(x-1)*3+y;
          for(int i=0;i<4;i++){
            int tx,ty;
            if(type[u])tx=x+dx2[i],ty=y+dy2[i];
            else tx=x+dx1[i],ty=y+dy1[i];
            int newpos=(tx-1)*3+ty;
            if(tx>0&&tx<=3&&ty>0&&ty<=3){
              next=cur;swap(next[pos],next[newpos]);
              v=encode(next);
              if(!vis[v]){
                vis[v]=1,fa[v]=u,step[v]=step[u]+1;
                type[v]=type[u]^1;
                opt[v]=make_pair(next[pos],next[newpos]);
                q.push(v);
              }
            }
          }
        }
      }
    }
  }
  void print_path(int u){
    if(fa[u]<0)return;
    print_path(fa[u]);
    printf("Step #%d: %d %d\n",step[u],opt[u].first,opt[u].second);
  }
};
solver g1,g2;

int main(){
  state st;
  for(int i=1;i<=9;i++)scanf("%d",&st[i]);
  g1.bfs(st,0);
  g2.bfs(st,1);
  if(!g1.vis[1]&&!g2.vis[1])printf("No Solution\n");
  else if(!g2.vis[1]||g1.step[1]<g2.step[1]){
    if(!g1.step[1])printf("No Need\n");
    else{g1.print_path(1);printf("\n");}
  }
  else{
    if(!g2.step[1])printf("No Need\n");
    else{g2.print_path(1);printf("\n");}
  }
  system("pause");
  return 0;
}
