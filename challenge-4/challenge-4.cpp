#include<cstdio>
#include<cmath>
#include<ctime>
#include<algorithm>
using namespace std;
const int maxn=1000+5;
const int INF=0x7fffffff;
const double EPS=1e-8;
const double pi=acos(-1);
#define RAND_UNIT (1.0*rand()/RAND_MAX)
#define RAND_INT (RAND_MAX<32768?rand()<<15|rand():rand())
#define RAND(L,R) (RAND_INT%((R)-(L)+1)+(L))
struct point{
  double x,y;
  point(double x=0,double y=0):x(x),y(y){}
  friend double dist(point a,point b){
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
  }
};
point p[maxn],ansp[maxn],v[4],ansv[4];
double r[maxn],cur,ans;
int seq[maxn],n=0,tot=0;
bool check(int id){
  for(int i=1;i<=n;i++)if(i!=id)
    if(dist(p[i],p[id])<r[i]+r[id]-EPS)return 0;
  return 1;
}
double f(){
  double A=-INF,B=INF,L=INF,R=-INF;
  for(int i=1;i<=n;i++){
    A=max(A,p[i].y+r[i]),B=min(B,p[i].y-r[i]);
    L=min(L,p[i].x-r[i]),R=max(R,p[i].x+r[i]);
  }
  v[0]=point(L,A),v[1]=point(L,B);
  v[2]=point(R,B),v[3]=point(R,A);
  return (A-B)*(R-L);
}
double search(){
  for(double t=20000;t>1e-10;t*=0.99996){
    int id;point tmp;
    for(;;){
      id=RAND(1,n),tmp=p[id];
      p[id].x+=RAND(-2000,2000)*t*0.0001;
      p[id].y+=RAND(-2000,2000)*t*0.0001;
      if(check(id))break;p[id]=tmp;
    }
    double d=f()-cur;
    if(d<0||exp(-d/t)>RAND_UNIT)cur+=d;
    else p[id]=tmp;
    if(cur<ans-EPS){
      ans=cur;
      for(int i=1;i<=n;i++)ansp[i]=p[i];
      for(int k=0;k<4;k++)ansv[k]=v[k];
    }
  }
  return ans;
}
int main(){
  srand(time(0));
  while(scanf("%d",&seq[++tot])==1);
  tot=tot-1>>1;
  for(int i=1;i<=tot;i++)
    for(int j=1;j<=seq[tot+i];j++){
      r[++n]=seq[i];
      ansp[n].x=p[n].x=p[n-1].x+r[n-1]+r[n];
  }
  cur=ans=f();
  for(int k=0;k<4;k++)ansv[k]=v[k];
  fprintf(stderr,"%.2lf\n",ans);
  for(int k=0;k<4;k++)fprintf(stderr,"%.2lf\n",search());
  printf("%.2lf\n",ans);
  return 0;
}
