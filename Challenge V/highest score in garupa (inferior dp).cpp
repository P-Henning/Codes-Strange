/*
input:
  2000 506 0.93 3
output:
  1142976.000
  1104444.000
*/
#include<cstdio>
const int maxk=1200+5,maxx=10+5;
const double EPS=1e-6;
const double INF=1e14;
double f[2][maxk][maxx][maxk],g[2][maxk][maxx][maxk];
double mf[2][maxk][maxx],mg[2][maxk][maxx];
double combo[maxk],p;
int a,k,x,ng;
inline void updatemax(double &a,double b){if(b>a)a=b;}
inline void updatemin(double &a,double b){if(b<a)a=b;}
int main(){
  scanf("%d%d%lf%d",&a,&k,&p,&x);
  for(int i=1;i<=20;i++)combo[i]=1;
  for(int i=21;i<=50;i++)combo[i]=1.01;
  for(int i=51;i<=100;i++)combo[i]=1.02;
  for(int i=101;i<=150;i++)combo[i]=1.03;
  for(int i=151;i<=200;i++)combo[i]=1.04;
  for(int i=201;i<=250;i++)combo[i]=1.05;
  for(int i=251;i<=300;i++)combo[i]=1.06;
  for(int i=301;i<=400;i++)combo[i]=1.07;
  for(int i=401;i<=500;i++)combo[i]=1.08;
  for(int i=501;i<=600;i++)combo[i]=1.09;
  for(int i=601;i<=700;i++)combo[i]=1.1;
  for(int i=701;i<=k;i++)combo[i]=1.11;
  ng=k-int(k*p+EPS)-x;
  for(int n=1;n<=k;n++){
    for(int b=0;b<=ng;b++)
      for(int c=0;c<=x;c++){
        int a=n-b-c;
        mf[n&1][b][c]=-INF,mg[n&1][b][c]=INF;
        for(int t=0;t<=a+b;t++)
          f[n&1][b][c][t]=-INF,g[n&1][b][c][t]=INF;
    }
    for(int b=0;b<=ng;b++)
      for(int c=0;c<=x;c++){
        int a=n-b-c;
        if(c)f[n&1][b][c][0]=mf[n&1][b][c]=mf[n&1^1][b][c-1],
             g[n&1][b][c][0]=mg[n&1][b][c]=mg[n&1^1][b][c-1];
        for(int t=1;t<=a+b;t++){
          if(a)updatemax(f[n&1][b][c][t],f[n&1^1][b][c][t-1]+1.1*combo[t]),
               updatemin(g[n&1][b][c][t],g[n&1^1][b][c][t-1]+1.1*combo[t]);
          if(b)updatemax(f[n&1][b][c][t],f[n&1^1][b-1][c][t-1]+0.8*combo[t]),
               updatemin(g[n&1][b][c][t],g[n&1^1][b-1][c][t-1]+0.8*combo[t]);
          updatemax(mf[n&1][b][c],f[n&1][b][c][t]),
          updatemin(mg[n&1][b][c],g[n&1][b][c][t]);
        }
    }
  }
  double maxans=-INF,minans=INF;
  for(int i=0;i<=k-x;i++){
    updatemax(maxans,f[k&1][ng][x][i]);
    updatemin(minans,g[k&1][ng][x][i]);
  }
  printf("%.3lf\n%.3lf\n",maxans*a,minans*a);
  return 0;
}
