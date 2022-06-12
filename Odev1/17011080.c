/*
Kodu terminal ekraninda calistirirken:
Ortalama filtresi icin mean, median icin median yaziyoruz.
Ardindan gelenler ise input ve output resimlerinin yollari.

./a.out mean input_resim_yolu output_resim_yolu
./a.out median input_resim_yolu output_resim_yolu
*/
#define MAXROWS 2000
#define MAXCOLS 2000
#define MAXLENGTH 256
#define MAXVALUE 255

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


int pgmRead (char *fileName,long *rows,long *cols, unsigned char image[MAXROWS][MAXCOLS]) {

  void lineControl(FILE *fp, char *line){
    while (line[0]=='#' || line[0]=='\n')
      fgets (line,MAXLENGTH,fp);
  }

  FILE *fp;
  char line[MAXLENGTH];
  int maximumValue = 0;
  int binary;
  long numberRead = 0;
  long i,j;
  int test,temp;



  if ((fp = fopen(fileName,"r")) == NULL) {
 printf ("Dosya Acilamadi\n\n");
 fclose (fp);
 return (0);
  }

  *cols = *rows =0;

  fgets (line,MAXLENGTH,fp);
  lineControl(fp, line);

  // P2 or P5
  if (line[0]=='P' && (line[1]=='2')) {
    binary = 0;
  }
  else if (line[0]=='P' && (line[1]=='5')) {
    binary = 1;

  }
  else {
    printf ("Dosya formati yanlis\n\n");
    fclose (fp);
    return (0);
  }


  fgets (line,MAXLENGTH,fp);
  lineControl(fp, line);
  sscanf (line,"%ld %ld",cols,rows);

  fgets (line,MAXLENGTH,fp);
  lineControl(fp, line);
  sscanf (line,"%d",&maximumValue);

  //P5 ten okuma
  if (binary) {
    for (i = 0; i < (*rows); i++) {
      numberRead += fread((void *)&(image[i][0]),
      sizeof(unsigned char),(*cols),fp);
      if (feof(fp)) break;
    }
  }

  // P2dan okuma
  else {
    for (i= 0; i < (*rows); i++) {
      for (j =0; j < (*cols); j++) {
        test = fscanf (fp,"%d",&temp);
        if (test == EOF) break;
        image[i][j] = (unsigned char)temp;

        numberRead++;
      }
      if (test == EOF) break;
    }
  }

  fclose (fp);
  return (1);
}

int pgmWrite(char* filename, long rows,long cols, unsigned char image[MAXROWS][MAXCOLS]) {
  FILE* file;
  int maxval;
  long nwritten = 0;
  long i,j;

  if ((file = fopen(filename, "w")) == NULL)	{
    printf("Dosya acilamadi.\n");
    return(0);
  }

  fprintf(file,"P5\n");
  fprintf(file,"%ld %ld \n", cols, rows);
  fprintf(file, "%d\n", (int)255);


  for (i=0; i < rows; i++) {
    nwritten += fwrite((void*)&(image[i][0]),sizeof(unsigned char),
		cols, file);
  }


  fclose(file);
  return(1);
}

int mean_filter(unsigned char  image[MAXROWS][MAXCOLS], long rows, long cols){


  unsigned char ext_image[MAXROWS][MAXCOLS];
  int i, j;

  // Window kenarlarda da calissin diye resmin kenarlarin ek satir  ve sutunlar ekledik
  for(i=0; i<rows; i++){
    ext_image[0][i] = 0;
    ext_image[i][0] = 0;
    ext_image[rows+1][i] = 0;
    ext_image[i][rows+1] = 0;
  }

  for(i=0; i<rows; i++)
    for(j=0; j<cols; j++){
      ext_image[i+1][j+1] = image[i][j];
    }


  for(i=0; i<rows; i++){
    for(j=0; j<cols; j++){
      image[i][j] = (
        ext_image[i][j]+
        ext_image[i][j+1]+
        ext_image[i][j+2]+
        ext_image[i+1][j]+
        ext_image[i+1][j+1]+
        ext_image[i+1][j+2]+
        ext_image[i+2][j]+
        ext_image[i+2][j+1]+
        ext_image[i+2][j+2]) / 9;
    }
  }

  return(1);

}


int median_filter(unsigned char  image[MAXROWS][MAXCOLS], long rows, long cols){

  unsigned char sort(unsigned char window[9]){
    int i, j, current_value;
    for (i = 1; i < 9; i++){
      current_value = window[i];
      j = i - 1;

      while (j >= 0 && window[j] > current_value)
      {
          window[j + 1] = window[j];
          j = j - 1;
      }
      window[j + 1] = current_value;
    }
    return window[4];
  }
  
  unsigned char ext_image[MAXROWS][MAXCOLS];
  unsigned char window[9];
  int i, j;


  // Window kenarlarda da calissin diye resmin kenarlarin ek satir  ve sutunlar ekledik
  for(i=0; i<rows; i++){
    ext_image[0][i] = 0;
    ext_image[i][0] = 0;
    ext_image[rows+1][i] = 0;
    ext_image[i][rows+1] = 0;
  }

  for(i=0; i<rows; i++)
    for(j=0; j<cols; j++){
      ext_image[i+1][j+1] = image[i][j];
    }


  for(i=0; i<rows; i++){
    for(j=0; j<cols; j++){
        window[0] = ext_image[i][j];
        window[1] = ext_image[i][j+1];
        window[2] = ext_image[i][j+2];
        window[3] = ext_image[i+1][j];
        window[4] = ext_image[i+1][j+1];
        window[5] = ext_image[i+1][j+2];
        window[6] = ext_image[i+2][j];
        window[7] = ext_image[i+2][j+1];
        window[8] = ext_image[i+2][j+2];


        image[i][j] = sort(window);
    }
  }

  return(1);

}


int main(int argc, char * argv[])
{
	char* ipfile;
  char* destfile;
  char* kernel_mode;

  kernel_mode = argv[1];
  ipfile = argv[2];
  destfile = argv[3];

  printf("kernel_mode : %s\n", kernel_mode);
  printf("input file : %s\n", ipfile);
  printf("dest file : %s\n", destfile);

  long rows, cols;
  rows=MAXROWS;
  cols=MAXCOLS;
  unsigned char image[MAXROWS][MAXCOLS];

  pgmRead(ipfile, &rows, &cols, image);

  if (strcmp(kernel_mode, "mean") == 0)
    mean_filter(image, rows, cols);
  else if (strcmp(kernel_mode, "median") == 0);
    median_filter(image, rows, cols);

  pgmWrite(destfile, rows, cols, image);

	return 0;
}
