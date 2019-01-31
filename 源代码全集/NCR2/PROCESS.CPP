#include <process.h>
#include <stdio.h>
#include <dir.h>
#include <string.h>
#include <conio.h>
int main()
 { char order[200];
   while(1)
    { system("ncr2.exe");
      FILE *fp = fopen("temp\\process\\editor.msg", "rt");
      if(fp==NULL)  return 0;
      strcpy(order, "tc  ");
      fscanf(fp, "%s\n", order+3);
      fclose(fp);
      chdir("compile");
      system(order);
      chdir("..");
    }
 }