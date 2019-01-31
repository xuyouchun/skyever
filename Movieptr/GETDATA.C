#include <stdio.h>

FILE *fp, *fp0, *bfp;
char source[20], object[20], bobject[20];
int  k, i, j, ch;
unsigned char dot[4][16], bdot[16];


main(int argc, char **argv)
 { if(argc!=2)
     { printf("Format for this: getdata sourcefile\n");
       exit(1);
     }
   strcpy(source, argv[1]);   strcat(source, ".ptr");
   strcpy(object, argv[1]);   strcat(object, ".txt");
   strcpy(bobject, argv[1]);  strcat(bobject, "_.txt");

   fp = fopen(source, "rb");
   if(fp==NULL)
    { printf("\nError: Can't open file \"%s\"!\n", source);
      exit(1);
    }
   fseek(fp, 2, SEEK_SET);
   for(j=0; j<4; j++) for(i=0; i<16; i++)
      dot[j][i] = fgetc(fp);
   fclose(fp);
   for(k=0; k<16; k++)  bdot[k] = dot[0][k]|dot[1][k]|dot[2][k]|dot[3][k];

   fp0 = fopen(object, "wt");
   if(fp0==NULL)
    { printf("\nError: Can't build file \"%s\"!\n", object);
      fcloseall();  exit(1);
    }


   printf("File: %s\n", object);
   fprintf(fp0, "{\n");  printf("{\n");
   for(j=0; j<4; j++)
    { fprintf(fp0, " { "); printf(" { ");
      for(i=0; i<16; i++)
       { ch = dot[j][i];
	 if(ch<=15)  { fprintf(fp0, "0x0%x", ch); printf("0x0%x", ch); }
	 else        { fprintf(fp0, "0x%x", ch); printf("0x%x", ch); }
	 if(i!=15)    { fprintf(fp0, ", "); printf(", "); }
	 if(i==7)  { fprintf(fp, "\n   ");  printf("\n   "); }
       }
      fprintf(fp0, " },\n"); printf(" },\n");
    }
   fprintf(fp0, "}");  printf("}");
   fclose(fp0);

   bfp = fopen(bobject, "wt");
   if(bfp==NULL)
    { printf("\nError: Can't build file \"%s\"!\n", bobject);
      fcloseall();  exit(1);
    }
   printf("\nFile: %s\n", bobject);
   fprintf(bfp, "{ ");   printf("{ ");
   for(k=0; k<16; k++)
    { ch = bdot[k];
      if(ch<=15)  { fprintf(bfp, "0x0%x", ch); printf("0x0%x", ch); }
      else        { fprintf(bfp, "0x%x", ch); printf("0x%x", ch); }
      if(k!=15)    { fprintf(bfp, ", "); printf(", "); }
      if(k==7)  { fprintf(fp, "\n  ");  printf("\n  "); }
    }
   fprintf(bfp, " }");  printf(" }");
   fclose(bfp);
   printf("\nOK!\n");
 }
