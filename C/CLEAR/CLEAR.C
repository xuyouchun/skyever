#include <stdio.h>
#include <dir.h>
#include <string.h>
int inkz(char *s, char *kz);
#define TRUE   1
#define FALSE  0

struct ffblk _ffblk;
int i;
int main(int num, char **S)
 {
  char path[80], filename[80];
  if(num == 1)
   {  puts("\nExample: clear c:\\windows\\recent  txt exe bmp");
      puts("This will clear the files in \"c:\\windows\\recent\", except *.txt *.exe and *.bmp");
      puts("     Write by XuYouChun ");
      puts("     2001.7.20  Ji Nan \n");
      exit(1);
   }
  strcpy(path, S[1]);
  if(S[1][strlen(S[1])-1] != '\\') strcat(path, "\\");
  strcat(path, "*.*");
  if(findfirst(path, &_ffblk, 0) == -1)
    exit(0);
  do
   { int Flag = FALSE;
     for(i=2; i<num; i++)
      if(inkz(_ffblk.ff_name, S[i]) == TRUE)  Flag = TRUE;
     if(Flag == TRUE) continue;
     strcpy(filename, S[1]);
     strcat(filename, "\\");
     strcat(filename, _ffblk.ff_name);
     unlink(filename);
   } while(findnext(&_ffblk)!=-1);
   exit(0);
 }

int inkz(char *s, char *kz)
 {
   int i=0;
   while(s[i]!='.')  if(s[i++] == '\0')  return FALSE;
   strupr(kz);
   if(strcmp(s+i+1, kz) == 0) return TRUE;
   return FALSE;
 }
