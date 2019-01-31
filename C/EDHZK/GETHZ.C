#include <stdio.h>

FILE *fp0, *fp;
int main(int argc, char **argv)
 { char ch;
   if(argc==1) { printf("source file missing!"); exit(1); }
   fp0 = fopen(argv[1], "rt");
   fp  = fopen("AAA.txt", "wt");
   while((ch=fgetc(fp0))!=EOF)  if(ch<0)  fputc(ch, fp);
   fcloseall();  return 0;
 }
   