#include <stdio.h>
#include <stdlib.h>
#include <string.h>

void main (){
  FILE *fin, *fo0, *fo1, *fo2, *fo3;
  int i = 0;
  char c;
  fin = fopen("quijote_enc_afin_var", "r");
  fo0 = fopen("quijote_enc_0", "w");
  fo1 = fopen("quijote_enc_1", "w");
  fo2 = fopen("quijote_enc_2", "w");
  fo3 = fopen("quijote_enc_3", "w");
  while((c = fgetc(fin)) != EOF){
    if(i == 0){
      fputc(c, fo0);
    }else if (i == 1){
      fputc(c, fo1);
    }else if (i == 2){
      fputc(c, fo2);
    }else if (i == 3){
      fputc(c, fo3);
    }
    i++;
    if(i == 4){
      i = 0;
    }
  }
  fclose(fin);
  fclose(fo0);
  fclose(fo1);
  fclose(fo2);
  fclose(fo3);
}
