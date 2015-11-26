#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>
#include <getopt.h>
#include <errno.h>
#include <sys/stat.h>

#define NAME_SIZE 256

#define EXITF(x)  { \
                     perror(x); \
                     exit(EXIT_FAILURE); \
                  }

int verbosity = 0;
char str[NAME_SIZE] = ".t2s";

int mode = 0;
/* 0 = default - same as 1, except append ".t2s"
   1 = append str to each input file name
   2 = create directory for the output files and preserve input file names */

int tabSize = 3;

void processFile(FILE* in, FILE* out)
{
   char c;
   int column = 0;
   
   while ((c = fgetc(in)) != EOF)
   {
      if (c == '\t')
      {
         if ((column % tabSize) == 0)
         {
            /* We just need to add `tabSize` spaces */
            
            for (int i = 0; i < tabSize; i++)
               fputc(' ', out);
            
            column += tabSize;
         }
         else
         {
            /* We need to align to the nearest `tabSize` */
            
            int spacesToAlign = tabSize - (column % tabSize);
            
            for (int i = 0; i < spacesToAlign; i++)
               fputc(' ', out);
            
            column += spacesToAlign;
         }
      }
      else if (c == '\n')
      {
         fputc(c, out);
         column = 0;
      }
      else
      {
         fputc(c, out);
         column++;
      }
   }
}

int main(int argc, char** argv)
{
   int opt;
   
   while ((opt = getopt(argc, argv, "vo:d:s:")) != -1)
   {
      if (opt == 'v')
         verbosity++;
      else if (opt == 'o')
      {
         strcpy(str, optarg);
         
         if (mode != 0)
         {
            fprintf(stderr, "Cannot specify -o and -f switches!\n");
            exit(EXIT_FAILURE);
         }
         
         mode = 1;
         
         if (verbosity > 0)
            printf("Rename mode selected\n");
      }
      else if (opt == 'd')
      {
         strcpy(str, optarg);
         
         if (mode != 0)
         {
            fprintf(stderr, "Cannot specify -o and -d switches!\n");
            exit(EXIT_FAILURE);
         }
         
         mode = 2;
         
         if (verbosity > 0)
            printf("Directory mode selected\n");
      }
      else if (opt == 's') 
      {
         tabSize = atoi(optarg);
         
         if (tabSize < 1)
         {
            fprintf(stderr, "Invalid tab size!\n");
            exit(EXIT_FAILURE);
         }
      }
   }
   
   if (optind == argc)
   {
      fprintf(stderr, "Usage: %s [-o ext=\".t2s\" | -d out_dir] [-s tab_size=3] "
              "input_files\n", argv[0]);
      return EXIT_FAILURE;
   }
   
   /* Firstly, if verbosity > 0, print files out */
   
   if (verbosity > 0)
   {
      printf("Files to be processed: ");
      
      int first = 1;
      
      for (int i = optind; i < argc; i++)
      {
         if (first == 1)
            first = 0;
         else
            putchar(' ');
         
         printf("%s", argv[i]);
      }
      
      putchar('\n');
   }
   
   if (mode == 2)
      if ((mkdir(str, 0700)) == -1)
         if (errno != EEXIST)
            EXITF("mkdir")
   
   int nextArg = optind;
   
   while (nextArg < argc)
   {
      if (verbosity > 0)
         printf("Converting \"%s\" -> ", argv[nextArg]);
      
      FILE* in = fopen(argv[nextArg], "r");
      
      if (in == NULL)
         EXITF("fopen")
      
      char outName[NAME_SIZE];
      
      if (mode == 2)
         sprintf(outName, "%s/%s", str, argv[nextArg]);
      else
         sprintf(outName, "%s.%s", argv[nextArg], str);
      
      if (verbosity > 0)
         printf("\"%s\"\n", outName);
      
      FILE* out = fopen(outName, "w");
      
      if (out == NULL)
         EXITF("fopen")
      
      processFile(in, out);
      
      if ((fclose(out)) == EOF)
         EXITF("fclose")
      
      if ((fclose(in)) == EOF)
         EXITF("fclose")
      
      nextArg++;
   }
   
   return EXIT_SUCCESS;
}
