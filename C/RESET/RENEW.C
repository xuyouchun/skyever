#include<stdio.h>
#include<string.h>
#include<dos.h>

char  sourcefile[50], objectfile[50];
FILE  *sourcefp, *objectfp;
void  help_msg()
 { printf("\nformat for this: RENEW sourcefile [objectfile]\n"); }
int main(int argc, char **argv)
 { int k, c;
   if(!(argc==2||argc==3))   { help_msg(); return 1; }
   strcpy(sourcefile, argv[1]);
   if(argc==3)  strcpy(objectfile, argv[2]);
   else
    { strcpy(objectfile, sourcefile);
      k = -1;
      while(objectfile[++k]!='.'&&objectfile[k]!='\0');
      strcpy(objectfile+k, ".exe");
    }
   sourcefp = fopen(sourcefile, "rt");
   if(sourcefp==NULL)
    { printf("\nCan't open sourcefile \"%s\"!", sourcefile);  return 1; }
   objectfp = fopen(objectfile, "wb");
   if(objectfp==NULL)
    { printf("\nCan't build objectfile \"%s\"!", objectfile); fcloseall(); return 1; }
   printf("\nPlease wait a while ...");  delay(500);
   while((c=fgetc(sourcefp))!='{'&&c!=EOF);
   while((c=fgetc(sourcefp))!=EOF)
    { int a, b;
      if(c=='\n'||c==' ')  continue;
      if(c=='}')   break;
      if(!(c>='A'&&c<='Z'))
       { puts("Error for reading!"); fcloseall(); return 1; }
      a = c; b = fgetc(sourcefp);
      if(!(b>='a'&&b<='z'))
       { puts("Error for reading!"); fcloseall(); return 1; }
      fputc((a-'A')*26+(b-'a'), objectfp);
    }
   fcloseall();
   printf("\nOK! object file name: %s", objectfile);
   return 0;
 }



