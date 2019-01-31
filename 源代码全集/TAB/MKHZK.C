#include <stdio.h>
#include <string.h>
int main(int argc, char **argv)
 { FILE *fpin, *fpout, *fphzk;
   int  dot, round;
   int  i, j, k;
   char inputname[80], outputname[80], hzkname[80];
   unsigned char buffer[2305];
   int  C0, C1;
   int  qh, wh;
   long  offset;
   long  py;
   if(argc != 4)
    {  puts("Format for this : mkhzk inputname outputname hzkname");
       exit(1);
    }
   strcpy(inputname, argv[1]);
   strcpy(outputname, argv[2]);
   strcpy(hzkname, argv[3]);

   if((fpin=fopen(inputname, "rb"))==NULL)
    { printf("Open file \"%s\" error!", inputname);
      exit(1);
    }
   if((fphzk=fopen(hzkname, "rb"))==NULL)
    { printf("Open file \"%s\" error!", hzkname);
      fcloseall();
      exit(1);
    }
   if((fpout=fopen(outputname, "wb"))==NULL)
    { printf("Open file \"%s\" error!", outputname);
      fcloseall();
      exit(1);
    }

   printf("DOT[16/24/32/40/48]: ");
   scanf("%d", &dot);
   printf("Offset[0/1410]: ");
   scanf("%ld", &py);

   C0 = fgetc(fpin);
   C1 = fgetc(fpin);

   while(C0+1 && C1+1)
    { qh = C0 - 0xa0;
      wh = C1 - 0xa0;
      offset = (94*(qh-1)+(wh-1) - py) * (dot*dot/8L);
      fseek(fphzk, offset, SEEK_SET);
      fread(buffer, dot/8, dot, fphzk);
      fwrite(buffer, dot/8, dot, fpout);
      C0 = fgetc(fpin);
      C1 = fgetc(fpin);
    }
   fcloseall();
 }
