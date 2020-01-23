#include<cstdio>
#include<cstring>
const int maxn=1e5+5;
const char *opt="><+-.,[]";
char code[maxn],tape[maxn],filename[maxn];
int match[maxn],len;
int st[maxn],stktop;
int errpos[maxn],postot;
// read_filename(endc): 读入文件名,读到endc字符时结束 
void read_filename(char endc){
  char c;int cnt=0;
  while((c=getchar())!=endc)filename[cnt++]=c;
  filename[cnt]=0;
}
// read_code(fin,endc): 从fin输入流中读入程序，读到endc字符时结束 
void read_code(FILE *fin,char endc){
  if(!fin){printf(" # Filename Error\n");return;}
  char c;
  while((c=fgetc(fin))!=endc){
    if(strchr(opt,c)){
      code[len++]=c;
      if(c=='[')st[stktop++]=len-1;
      if(c==']'){
        if(!stktop)errpos[postot++]=len-1;
        else{
          match[len-1]=st[stktop-1];
          match[st[--stktop]]=len-1;
        }
      }
    }
  }
  for(int i=0;i<stktop;i++)errpos[postot++]=st[i];
}
// run_code: 运行程序 
void run_code(){
  if(postot){
    printf(" # Unmatched Brackets at ");
    for(int i=0;i<postot;i++)
      printf("%d%s",errpos[i],i==postot-1?"\n":", ");
    return;
  }
  if(!len)return;
  char *ptr=tape+maxn/2,c;bool eof=0;
  for(int i=0;i<len;i++)switch(code[i]){
    case '>':ptr++;break;
    case '<':ptr--;break;
    case '+':(*ptr)++;break;
    case '-':(*ptr)--;break;
    case '.':putchar(*ptr);break;
    case ',':
      if(!eof)c=getchar();
      if(c=='\n')eof=1;else *ptr=c;
      break;
    case '[':if(!(*ptr))i=match[i];break;
    case ']':if(*ptr)i=match[i]-1;break;
  }
  printf("\n");
}
// input_code(): 读入程序 
void input_code(){
  char type;while((type=getchar())<=32);
  if(type=='@'){
    read_code(stdin,'@');getchar();
  }
  if(type=='$'){
    read_filename('$');getchar();
    FILE *fin=fopen(filename,"r");
    read_code(fin,-1);
    if(fin)fclose(fin);
  }
}
int main(){
  for(;;){
    printf(" # BrainFuck Code (Begin and End with \'@\')\n");
    printf(" # Or Filename    (Begin and End with \'$\')\n");
    printf(" # Example: @,<,>[-<+>]++++++[-<-------->]<.@\n");
    printf("            $code.bf$\n");
    memset(tape,0,sizeof(tape));
    stktop=postot=len=0;
    input_code();run_code();
  }
  return 0;
}
