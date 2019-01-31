#include <stdio.h>
#include <string.h>

FILE *fp;
main(int argc, char **argv)
 { char filename[30][30];
   int k, ch;
   int num = argc - 1;
   long linenum = 0, yznum = 0;
   if(argc==1)
     { printf("ERROR:  NO  FILE !");
       exit(1);
     }
   for(k=1; k<argc; k++)
     strcpy(filename[k-1], argv[k]);
   for(k=0; k<num; k++)
     { printf("%s\n", filename[k]);
       fp = fopen(filename[k], "rb");
       if(fp==NULL)
	{ printf("Can't open file \"%s\"!", filename[k]);
	  exit(1);
	}
       while( (ch=fgetc(fp)) != EOF)
	{ if(ch==13)   linenum++;
	  if(ch==';')  yznum++;
	}
       fclose(fp);
     }
   printf("LINE: %ld\n", linenum);
   printf("WORD: %ld\n", yznum);
 }


