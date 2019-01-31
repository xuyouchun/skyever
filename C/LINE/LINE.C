#include <stdio.h>
#include <string.h>
#include <dir.h>

FILE *fp;
int  k, ch;
char filename[20][50];
long line, Line;
struct ffblk Ffblk;

int main(int argc, char **argv)
 { if(argc==1)
    { printf("Format for this: LINE filelist [filelist...]");
      return 1;
    }
   for(k=0; k<argc-1; k++)  strcpy(filename[k], argv[k+1]);
   for(k=0; k<argc-1; k++)
    { if(findfirst(filename[k], &Ffblk, 0)==-1)  continue;
       do
	{ fp = fopen(Ffblk.ff_name, "rt");
	  if(fp==NULL)
	   { printf("ERROR: Can't open file %s!", Ffblk.ff_name);
	     return 1;
	   }
	  line = 0;
	  while( (ch=fgetc(fp))!=EOF )   if(ch==';')  line ++;
	  fclose(fp);
	  printf("\nFile: %-13s  ->  line: %-10ld", Ffblk.ff_name, line);
	  Line += line;
	} while(findnext(&Ffblk)!=-1);
    }
   printf("\n=======>>>>> %10ld lines in all!", Line);
   return 0;
 }
       