#include<cstdio>
const char *str[]={"m","gr","p"};
const int maxk=1200+5,maxx=10+5;
const double EPS=1e-6;
const double INF=1e14;
double f[maxx][maxk][maxk];
double d[maxk][maxk],combo[maxk],p;
int type[maxk],cnt[maxk],tot;
int h1[maxx][maxk][maxk],h2[maxx][maxk][maxk];
int A,k,x,np,ng;
double tsearch2(int m,int a,int b,int t1){
  int l=t1?0:1,r=b;double ans=INF;
  while(r-l>10){
    int lmid=l+(r-l)/3,rmid=l+(r-l)*2/3;
    double f1=f[m-1][a-t1][b-lmid]+d[t1][lmid];
    double f2=f[m-1][a-t1][b-rmid]+d[t1][rmid];
    if(f1>=INF||f1>=f2)l=lmid;
    else r=rmid;
  }
  for(int i=l;i<=r;i++){
    double f0=f[m-1][a-t1][b-i]+d[t1][i];
    if(f0<ans)ans=f0,h2[m][a][b]=i;
  }
  return ans;
}
double tsearch1(int m,int a,int b){
  int l=b?0:1,r=a;double ans=INF;
  while(r-l>10){
    int lmid=l+(r-l)/3,rmid=l+(r-l)*2/3;
    double f1=tsearch2(m,a,b,lmid);
    if(f1>=INF||f1>=tsearch2(m,a,b,rmid))l=lmid;
    else r=rmid;
  }
  for(int i=l;i<=r;i++){
    double f0=tsearch2(m,a,b,i);
    if(f0<ans)ans=f0,h1[m][a][b]=i;
  }
  return ans;
}
int main(){
  scanf("%d%lf%d%d",&A,&p,&k,&x);
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
  np=int(k*p+EPS),ng=k-np-x;

  double ans=0;tot=0;
  for(int i=1;i<=k-x;i++){
    if(i<=ng)ans+=0.8*combo[i];
    else ans+=1.1*combo[i];
  }
  printf("%.3lf\n",ans*A);
  if(ng)type[++tot]=1,cnt[tot]=ng;
  if(np)type[++tot]=2,cnt[tot]=np;
  if(x)type[++tot]=0,cnt[tot]=x;
  for(int i=1;i<=tot;i++){
    if(i>1)putchar('-');
    printf("%s",str[type[i]]);
    if(cnt[i]>1)printf("%d",cnt[i]);
  }
  printf("\n");

  for(int a=0;a<=np;a++)
    for(int b=0;b<=ng;b++)
      if(a||b)f[0][a][b]=INF;
  for(int t1=0;t1<=np;t1++)
    for(int t2=0;t2<=ng;t2++)if(t1||t2){
      if(!t2)d[t1][t2]=d[t1-1][0]+1.1*combo[t1];
      else d[t1][t2]=d[t1][t2-1]+0.8*combo[t1+t2];
  }
  for(int m=1;m<=x+1;m++)
    for(int a=0;a<=np;a++)
      for(int b=0;b<=ng;b++)
        if(a||b)f[m][a][b]=tsearch1(m,a,b);
  int c,c1=np,c2=ng;ans=INF,tot=0;
  for(int i=1;i<=x+1;i++)
    if(f[i][np][ng]<ans)ans=f[i][np][ng],c=i;
  printf("%.3lf\n",ans*A);
  for(int i=c;i>=1;i--){
    int t1=h1[i][c1][c2],t2=h2[i][c1][c2];
    if(t1)type[++tot]=2,cnt[tot]=t1;
    if(t2)type[++tot]=1,cnt[tot]=t2;
    if(i>1)type[++tot]=0,cnt[tot]=1,x--;
    else if(x)type[++tot]=0,cnt[tot]=x;
    c1-=t1,c2-=t2;
  }
  for(int i=1;i<=tot;i++){
    if(i>1)putchar('-');
    printf("%s",str[type[i]]);
    if(cnt[i]>1)printf("%d",cnt[i]);
  }
  printf("\n");
  return 0;
}
