#include<stdio.h>
#include<string.h>
#include<dos.h>

char  sourcefile[50], objectfile[50];
FILE  *sourcefp, *objectfp;
void  help_msg()
 { printf("\nformat for this: RESET sourcefile [objectfile]\n"); }
int main(int argc, char **argv)
 { int c, K; char ch;
   if(!(argc==2||argc==3))   { help_msg(); return 1; }
   strcpy(sourcefile, argv[1]);
   if(argc==3)  strcpy(objectfile, argv[2]);
   else
    { int k = -1;
      strcpy(objectfile, sourcefile);
      while(objectfile[++k]!='.'&&objectfile[k]!='\0');
      strcpy(objectfile+k, ".txt");
    }
   sourcefp = fopen(sourcefile, "rb");
   if(sourcefp==NULL)
    { printf("\nCan't open sourcefile \"%s\"!", sourcefile);  return 1; }
   objectfp = fopen(objectfile, "wt");
   if(objectfp==NULL)
    { printf("\nCan't build objectfile \"%s\"!", objectfile); fcloseall(); return 1; }
   printf("Please wait a while...");  delay(500);
   K = 0;
   fprintf(objectfp, "THE BEGIN OF FILE!\n{\n");
   while((c=fgetc(sourcefp))!=EOF)
    { fputc(c/26+'A', objectfp);
      fputc(c%26+'a', objectfp);
      if(++K==38)
	{ fputc('\n', objectfp); K = 0; }
    }
   fprintf(objectfp, "\n}\nTHE END OF FILE!\n");
   fcloseall();
   printf("\nOK! object file name: %s", objectfile);
   return 0;
 }



