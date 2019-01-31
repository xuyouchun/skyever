#include <dos.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

char  *sourcedir;
char  objectdir[15][20], objectfile[100];
void  addfile(struct ffblk *);
FILE  *fp;  int filen, dirn;
char  keywords[50];  int len, keyflag, flag, flag0;
struct ftime ftimep;
unsigned *fptr = (unsigned *)&ftimep;
void  writeinfile(struct ffblk *, int N);


int main(int argc, char **argv)
 { int k, K=1;
   if(argc==1) ac:{ printf("ADD (source file or dir list) [/K(key words)] [/(object file name)] \n");  exit(1); }
   strcpy(objectfile, argv[1]);
   for(k=1; k<argc; k++)  
    { if(strcmp(argv[k], "/K")==0 || strcmp(argv[k], "/k")==0)  flag = 1;
      else if(argv[k][0]=='/')  { strcpy(objectfile, argv[k]+1); flag0 = 1; }
      else strcpy(objectdir[K++], argv[k]);  
    }
   if(K==1)  goto ac;
   reopenfile:
   if(access(strcat(objectfile, ".add"), 0)==0)
     { char ch;
       printf("File %s had already exist, overwrite it?(Y/N)", objectdir);
       if((ch=getch())!='Y'&&ch!='y')  exit(1);    putchar('\n');
     }
   fp = fopen(objectfile, "wb");
   if(fp==NULL)
    { printf("please input the object file name:"); 
      scanf("%s", objectfile);  goto reopenfile;
    }
   if(flag==1)
    { printf("Please enter the keywords:");
      scanf("%s", keywords);       fputc('@', fp);
    }
   else  
    { strcpy(keywords, "ÈËÉúÈçÃÎskyever");   fputc('#', fp);  }
   len = strlen(keywords); 
   for(k=1; k<argc-flag-flag0; k++)      
    { struct ffblk Ffblk;  
      if(findfirst(objectdir[k], &Ffblk, FA_DIREC|FA_HIDDEN|FA_RDONLY|FA_SYSTEM)!=-1);
	do addfile(&Ffblk);  while(findnext(&Ffblk)!=-1);
    }
   fprintf(fp, "0000*!\n?\n%ld,0\n", 0L);   fclose(fp);   
   printf("************** THE END:\a%d file(s)  %d dir(s) --> %s!\n", 
	   filen, dirn, objectfile);
   printf("                    >>>>>>>>> Skyever <<<<<<<<<<");
   return 0;
 }

void  addfile(struct ffblk *ff)
 { int  N = 0; FILE *fp0;     int  handle;   
   struct ffblk Ffblk;  int code;  char attrib = ff->ff_attrib;   
   if(attrib&FA_DIREC)  
     { sourcedir = ff->ff_name; chdir(sourcedir); dirn++; }
   else  { sourcedir = "(CURPATH)"; writeinfile(ff, 1); return; }
   findfirst("*.*", &Ffblk, FA_DIREC|FA_HIDDEN|FA_RDONLY|FA_SYSTEM);
   findnext(&Ffblk);   code = findnext(&Ffblk);
   if(code==-1)  fprintf(fp, "0000*%s\n?\n0,0\n", sourcedir);
   else 
    { do
       { if(Ffblk.ff_attrib & FA_DIREC)  
	   { if(N++==0)  fprintf(fp, "0000*%s\n?\n0,0\n", sourcedir);
	     addfile(&Ffblk);  sourcedir = ff->ff_name; 
	   }
	 else  writeinfile(&Ffblk, N++);  
       } while(findnext(&Ffblk)!=-1);
    }
   fprintf(fp, "0000*..\n?\n0,0\n");   chdir("..");
 }

void  writeinfile(struct ffblk *Ffblk, int N)
 { FILE *fp0;   int handle;  long size, k;
   printf("NO.%-4d ->Add file ", filen+1);
   textcolor(LIGHTGREEN); cprintf("%-13s", Ffblk->ff_name); textcolor(LIGHTGRAY);
   printf("...File lenth: %-9ld Path: %-13s\n", Ffblk->ff_fsize, sourcedir);
   fp0 = fopen(Ffblk->ff_name, "rb");
   if(fp0==NULL)
     { printf("Error: Can't open file %s!", Ffblk->ff_name);
       fcloseall(); exit(1); 
     }
   handle = fileno(fp0);	 getftime(handle, &ftimep);
   fprintf(fp, "0000*%s\n%s\n%ld,%c,%d,%d\n0000*", N?".":sourcedir, 
      Ffblk->ff_name, size=Ffblk->ff_fsize, Ffblk->ff_attrib, fptr[0], fptr[1]);
   for(k=0; k<size; k++)  
      fputc(keywords[keyflag=(keyflag++==len?0:keyflag)]^fgetc(fp0), fp);
   fclose(fp0);  filen ++;
 }
