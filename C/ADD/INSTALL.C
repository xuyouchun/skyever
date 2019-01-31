#include <dos.h>
#include <dir.h>
#include <string.h>
#include <stdio.h>
#include <io.h>
#include <conio.h>

char  sourcedir[13], curpath[100]="X:\\";
char  sourcefile[15][20];
void  installfile(char *objectfile);
FILE  *fp;   int filen, dirn;
int   keyflag, len;
char  keywords[50];
struct ftime ftimep;
unsigned *fptr = (unsigned *)&ftimep;

int main(int argc, char **argv)
 { int k, K=1;   
   if(argc==1)  findf:
     { struct  ffblk  Ffblk;
       if(findfirst("*.add", &Ffblk, 0)==-1)  
	 { printf("Can't find any add file!\n");  exit(1); }
       do strcpy(sourcefile[K++], Ffblk.ff_name);
	  while(findnext(&Ffblk)!=-1);
     }
   else for(k=1; k<argc; k++)   
     { if(strcmp(strupr(&argv[k][strlen(argv[k])-4]), ".EXE")==0)  goto findf;
       strcpy(sourcefile[K++], argv[k]);   
     }
   *curpath = 'A'+getdisk();    getcurdir(0, curpath+3);  
   if(curpath[strlen(curpath)-1]=='\\')  curpath[strlen(curpath)-1] = '\0';
   if(getdisk()<=1)  
    { setdisk(2);
      if(chdir("c:\\windows\\desktop")==-1 && chdir("C:\\")==-1)
	{ setdisk(3); chdir("D:\\"); }
    }   
   for(k=1; k<K; k++)   
     { char path[200]; int i;
       for(i=strlen(sourcefile[k])-1; i>=0; i--)  if(sourcefile[k][i]=='\\') break;
       if(i==-1) sprintf(path, "%s\\%s", curpath, sourcefile[k]);
       else      strcpy(path, sourcefile[k]); 
       installfile(path);
     }
   printf("************** THE END:\a%d file(s)  %d dir(s) \n", filen, dirn);
   cprintf("                    >>>>>>>>> Skyever <<<<<<<<<<");
   return 0;
 }

void  installfile(char *sourcefile)
 { FILE  *fp0;  char filename[21], dirname[21]; long size, k;  char attrib;
   int   handle;
   for(k=0; k<strlen(sourcefile); k++)  if(sourcefile[k]=='.')  break;
   if(k==strlen(sourcefile))  strcat(sourcefile, ".add");
   fp = fopen(sourcefile, "rb");
   if(fp==NULL)
    { printf("Error: Can't open file %s!", sourcefile); return; }
   if(fgetc(fp)=='@')  { printf("Please enter the keywords:");  scanf("%s", keywords); }
   else  strcpy(keywords, "ÈËÉúÈçÃÎskyever");
   len = strlen(keywords);   keyflag = 0;
   do
    { while(fgetc(fp)!='*');
      fgets(dirname, 20, fp);  fgets(filename, 20, fp);  
      fscanf(fp, "%ld,%c,%d,%d\n", &size, &attrib, fptr, fptr+1);
      dirname[strlen(dirname)-1] = 0;  filename[strlen(filename)-1] = 0;
      if(*dirname=='!')  break;
      if(*filename=='?')
	{ if(strcmp(dirname, "..")==0)   
	   { int k;  char str[100];  chdir("..");   getcurdir(0, str);
	     for(k=strlen(str)-1; k>0; k--)  if(str[k]=='\\') break;
	     if(k>=0) strcpy(sourcedir, str+k+1); 
	     else     strcpy(sourcedir, "(CURPATH)");
	   }
	  else { mkdir(dirname); chdir(dirname); strcpy(sourcedir, dirname); dirn++; } continue;
	}
      if(strcmp(dirname, "."))  {  mkdir(dirname);  chdir(dirname); strcpy(sourcedir, dirname); dirn++; }
      printf("NO.%-4d ->Build file ", filen+1);
      textcolor(LIGHTGREEN);  cprintf("%-13s", filename);  textcolor(LIGHTGRAY);
      printf("...File lenth: %-9ld Path: %-13s\n", size, sourcedir);
      fp0 = fopen(filename, "wb");
      if(fp0==NULL)  
	{ printf("Error: Can't build file %s!\n", filename); fcloseall(); exit(1); }
      while(fgetc(fp)!='*');
      for(k=0; k<size; k++)  
	fputc(fgetc(fp)^keywords[keyflag=(keyflag++==len?0:keyflag)], fp0);
      handle = fileno(fp0);    setftime(handle, &ftimep);
      fclose(fp0);  filen ++;     _chmod(filename, 1, attrib);
    } while(1);
   fclose(fp);
 }

