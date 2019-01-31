#include <stdio.h>
#include <io.h>

FILE *fp, *fp0;
char sourcefile[50], objectfile[50], fntfile[50];

int main(int argc, char **argv)
 { int k, num;
   if(argc==1) { printf("Error: source txt missing!\n");  return 1; }
   strcpy(sourcefile, argv[1]);
   for(k=strlen(sourcefile)-1; k>=0; k--)  if(sourcefile[k]=='.') break;
   if(k<0)  strcat(sourcefile, ".TXT");
   if(argc>=3)  
    { strcpy(objectfile, argv[2]);
      for(k=strlen(objectfile)-1; k>=0; k--)  if(objectfile[k]=='.') break;
      if(k<0)  strcat(objectfile, ".SRC");
    }
   else       
     { strcpy(objectfile, sourcefile);
       for(k=strlen(objectfile)-1; k>=0; k--)  if(objectfile[k]=='.') break;
       strcpy(objectfile+k, ".SRC");
     }
   strcpy(fntfile, sourcefile);
   for(k=strlen(fntfile)-1; k>=0; k--)  if(fntfile[k]=='.') break;
   strcpy(fntfile+k, ".FNT");

   fp0 = fopen(sourcefile, "rt");
   if(fp0==NULL)
    { printf("Error: Can't open source file: %s!\n", sourcefile);  return 1; }
   if(access(objectfile,0)==0)
    { char ch;
      printf("File %s had already exist, overight it?(Y/N)", objectfile);
      if((ch=getch())!='Y'&&ch!='y') { fcloseall(); exit(1); }  putchar('\n');
    }
   fp = fopen(objectfile, "wt");
   if(fp==NULL)
    { printf("Error: Can't open object file: %s!\n", objectfile); fcloseall(); return 1; }
   num = (int)filelength(fileno(fp0))/2;
   fprintf(fp, "object name: %s\n", fntfile==NULL?"NULL":fntfile);
   fprintf(fp, "font in use: 1\n");
   fprintf(fp, "<NO. 1>--filename: HZK16 index: 0 offset: 0\n");
   fprintf(fp, "font dot: 16\n");
   fprintf(fp, "font num: %d\n", num+1);
   fprintf(fp, "font->index->round\n");
   for(k=0; k<num; k++)
    { char ch1=fgetc(fp0), ch2=fgetc(fp0);
      fprintf(fp, "%c%cAA", ch1&0x7f, ch2&0x7f);
    }
   fcloseall(); 
   printf("OK! %s -> %s", sourcefile, objectfile); 
   return 0;
 }


