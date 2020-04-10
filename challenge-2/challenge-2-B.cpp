#include<cstdio>
#include<cmath>
#include<algorithm>
using namespace std;
const int INF=0x7fffffff;
const double EPS=1e-6;
const double pi=acos(-1);
const double sin60=sin(pi/3);
const double cos60=cos(pi/3);
const double tan60=tan(pi/3);
// double_to_int: 万恶的浮点数误差 
int double_to_int(double x){
  if(x>0)x+=EPS;else x-=EPS;
  return (int)x;
}
// point: 坐标系中的点
struct point{
  double x,y;
  point(double x=0,double y=0):x(x),y(y){}
  // dist(a,b): 点a,b的欧几里得距离 
  friend double dist(point a,point b){
    return sqrt((a.x-b.x)*(a.x-b.x)+(a.y-b.y)*(a.y-b.y));
  }
  // convect(p1,p2): 与p1p2同向的邻接向量 
  // p+q*cos60 = p2.x-p1.x
  //   q*sin60 = p2.y-p1.y
  // d = (p2-p1)/gcd(p,q)
  friend point convect(point p1,point p2){
    int p=double_to_int(p2.x-p1.x-(p2.y-p1.y)/tan60);
    int q=double_to_int((p2.y-p1.y)/sin60);
    int gcd=__gcd(abs(p),abs(q));
    return point((p2.x-p1.x)/gcd,(p2.y-p1.y)/gcd);
  }
};
// coord(n0,a): 第n0层第a个点的坐标 
//   r^2 = n0^2+(a-1 mod n0)^2-2*n0(a-1 mod n)cos60
// theta = [(a-1)/n0](pi/3)+acos((n0-(a-1 mod n0)cos60)/r)
// coord(n0,a) = (r*cos(pi*2/3-theta),r*sin(pi*2/3-theta))
point coord(int n0,int a){
  if(!n0)return point(0,0);
  int left=(a-1)%n0;
  double r=sqrt(n0*n0+left*left-2*n0*left*cos60);
  double theta=floor((a-1)/n0)*(pi/3)+acos((n0-left*cos60)/r);
  return point(r*cos(pi*2/3-theta),r*sin(pi*2/3-theta));
}
// extend(p1,p2,n): 延长p1p2使之不超过第n层,返回cnt
// d = convect(p1,p2)
// |(p2.x+cnt*d.x)tan60-(p2.y+cnt*d.y)| <= n*tan60
// |(p2.x+cnt*d.x)tan60+(p2.y+cnt*d.y)| <= n*tan60
//                       |p2.y+cnt*d.y| <= n*sin60
int extend(point p1,point p2,int n){
  point d=convect(p1,p2);
  double t1=INF,b1=-INF,t2=INF,b2=-INF,t3=INF,b3=-INF;
  int cnt=INF;
  if(fabs(d.x*tan60-d.y)>EPS){
    t1=(n*tan60-p2.x*tan60+p2.y)/(d.x*tan60-d.y);
    b1=(-n*tan60-p2.x*tan60+p2.y)/(d.x*tan60-d.y);
  }
  if(fabs(d.x*tan60+d.y)>EPS){
    t2=(n*tan60-p2.x*tan60-p2.y)/(d.x*tan60+d.y);
    b2=(-n*tan60-p2.x*tan60-p2.y)/(d.x*tan60+d.y);
  }
  if(fabs(d.y)>EPS){
    t3=(n*sin60-p2.y)/d.y;
    b3=(-n*sin60-p2.y)/d.y;
  }
  if(d.x*tan60-d.y<-EPS)swap(b1,t1);
  if(d.x*tan60+d.y<-EPS)swap(b2,t2);
  if(d.y<-EPS)swap(t3,b3);
  if(b1-EPS<t1)cnt=min(cnt,(int)floor(t1+EPS));
  if(b2-EPS<t2)cnt=min(cnt,(int)floor(t2+EPS));
  if(b3-EPS<t3)cnt=min(cnt,(int)floor(t3+EPS));
  return max(cnt,0);
}
// ans = gcd(p,q)+cnt+1
int main(){
  int n,n1,a1,n2,a2;
  scanf("%d",&n);
  while(scanf("%d-%d to %d-%d",&n1,&a1,&n2,&a2)==4){
    point p1=coord(n1,a1),p2=coord(n2,a2);
    int p=double_to_int(p2.x-p1.x-(p2.y-p1.y)/tan60+EPS);
    int q=double_to_int((p2.y-p1.y)/sin60+EPS);
    printf("%d\n",__gcd(abs(p),abs(q))+extend(p1,p2,n)+1);
  }
  return 0;
}
