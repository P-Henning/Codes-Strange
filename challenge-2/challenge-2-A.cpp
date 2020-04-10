#include<cstdio>
#include<cmath>
#include<algorithm>
using namespace std;
const int INF=0x7fffffff;
const double EPS=1e-6;
// point: 坐标系中的点
struct point{
  int x,y;
  point(int x=0,int y=0):x(x),y(y){}
  // dist(a,b): 点a,b的曼哈顿距离 
  friend int dist(point a,point b){
    return abs(a.x-b.x)+abs(a.y-b.y);
  }
  // convec(p1,p2): 与p1p2共线的邻接向量 
  friend point convec(point p1,point p2){
    int dx=p2.x-p1.x,dy=p2.y-p1.y,gcd=__gcd(abs(dx),abs(dy));
    return point(dx/gcd,dy/gcd);
  }
};
// coord(n0,a): 第n0层第a个点的坐标 
// coord(n0,a) = (a-1,n0-a+1)           0<a<=n0
// coord(n0,a) = (2n0-a+1,n0-a+1)      n0<a<=2n0
// coord(n0,a) = (2n0-a+1,-3n0+a-1)   2n0<a<=3n0
// coord(n0,a) = (-4n0+a-1,-3n0+a-1)  3n0<a<=4n0 
point coord(int n0,int a){
  if(!n0)return point(0,0);
  if(a<=n0)return point(a-1,n0-a+1);
  if(a<=2*n0)return point(2*n0-a+1,n0-a+1);
  if(a<=3*n0)return point(2*n0-a+1,-3*n0+a-1);
  return point(-4*n0+a-1,-3*n0+a-1);
}
// extend(p1,p2,n): 延长p1p2使之不超过第n层,返回cnt
// d = convec(p1,p2)
// |p2.x+cnt*d.x|+(p2.y+cnt*d.y) <= n
// |p2.x+cnt*d.x|-(p2.y+cnt*d.y) <= n
int extend(point p1,point p2,int n){
  point d=convec(p1,p2);
  double t1=INF,b1=-INF,t2=INF,b2=-INF;
  int cnt=INF;
  if(d.x+d.y){
    t1=(double)(n-p2.x-p2.y)/(d.x+d.y);
    b1=(double)(-n-p2.x-p2.y)/(d.x+d.y);
  }
  if(d.x-d.y){
    t2=(double)(n-p2.x+p2.y)/(d.x-d.y);
    b2=(double)(-n-p2.x+p2.y)/(d.x-d.y);
  }
  if(d.x+d.y<0)swap(b1,t1);
  if(d.x-d.y<0)swap(b2,t2);
  if(b1-EPS<t1)cnt=min(cnt,(int)floor(t1+EPS));
  if(b2-EPS<t2)cnt=min(cnt,(int)floor(t2+EPS));
  return max(cnt,0);
}
// ans = gcd(p1p2.x,p1p2.y)+cnt+1
int main(){
  int n,n1,a1,n2,a2;
  scanf("%d",&n);
  while(scanf("%d-%d to %d-%d",&n1,&a1,&n2,&a2)==4){
    point p1=coord(n1,a1),p2=coord(n2,a2);
    printf("%d\n",__gcd(abs(p1.x-p2.x),abs(p1.y-p2.y))+extend(p1,p2,n)+1);
  }
  return 0;
}
