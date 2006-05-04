/*------------------------------------------*
 * The STRIPPER by Dilwyn Jones/Phil Borman *
 *                                          *
 * From IQLR volume 3 issue 4.              *
 *------------------------------------------*
 * Modified by Norman Dunbar to :-          *
 *                                          *
 * 1. Do away with replacement text for the *
 *    Quill special codes. It becomes a text*
 *    extractor only.                       *
 *                                          *
 * 2. Use a command line argument for the   *
 *    files to be converted. Allows the use *
 *    of wildcards.                         *
 *                                          *
 * 3. Wrap lines around at column 80.       *
 *------------------------------------------*/

#include <stdio_h>
#include <stdlib_h>
#include <string_h>
#include <qdos_h>

void consetup_title();
void (*_consetup)() = consetup_title;

char _prog_name[] = "The STRIPPER";
char _version[] = "1.01";
char _copyright[] = "DJ, PB & ND, 1993-94";
void (*_cmdwildcard)() = cmdexpand;

#define MAX_WIDE 80

long getline(char *, long, FILE *);
long getbuff(char *, long, FILE *);


void main(int argc, char *argv[])
{
   char filename[32],
        output[32],
        header[120],
        footer[120],
        buffer[64];

   FILE *fp, *op;

   int  err,
        file,
        wide,
        ch;

   long fsize,
        txtlen,
        endpos,
        curpos,
        f;

   if (argc < 3) {
       printf("EX STRIPPER_EXE;'list_of_files output_device'\n");
       printf("\nList_of_files = device_file_DOC or device_wildcard_doc");
       printf("\noutput_device = where to put converted files\n");
       printf("\nEXAMPLES :\n");
       printf("EX STRIPPERS_EXE;'flp1_*_doc ram1_' ");
       printf("converts all doc files from flp1_ to ram1_\n");
       printf("EX STRIPPER_EXE;'flp1_help_doc flp1_fred_doc ram1_' ");
       printf("converts help_doc and fred_doc from flp1_ to ram1_\n\n");
       exit(0);
   }

   sd_clear(fgetchid(stdout), -1);

   for (file = 1; file < argc -1; file++) {
       strcpy(filename, argv[file]);
       printf("%s --> ", filename);

       fp = fopen(argv[file], "r");
       if (fp == NULL) {
           printf("can't be opened.\n");
           continue;
       }

       err = fs_headr(fgetchid(fp), -1, buffer, 64);
       if (err < 0) {
           printf("error %d, reading header.\n", err);
           fclose(fp);
           continue;
       }

       fsize = *((long *)buffer);

       err = fgetc(fp);
       err = fgetc(fp);

       txtlen = getbuff(buffer, 9, fp);
       if (!txtlen) {
           printf("read error.\n");
           fclose(fp);
           continue;
       }

       err = strcmp(buffer, "vrm1qdf0");
       if (err) {
           printf("not a Quill file.\n");
           fclose(fp);
           continue;
       }

       txtlen = getbuff(buffer, 5, fp);
       if (!txtlen) {
           printf("read error %s\n", filename);
           fclose(fp);
           continue;
       }

       endpos = *((long *)buffer);

       for (f = 0; f < 6; err = fgetc(fp), f++);

       curpos = 20;
       txtlen = getline(header, 120, fp);
       if (!txtlen) {
           printf("read error.\n");
           fclose(fp);
           continue;
       }

       curpos += txtlen;
       txtlen = getline(footer, 120, fp);
       if (!txtlen) {
           printf("read error.\n");
           fclose(fp);
           continue;
       }

       curpos += txtlen;

       strcpy(output, argv[argc-1]);
       strncat(output, filename +5, strlen(filename) -9);
       strcat(output, "_txt");
       printf("%s\n", output);

       op = fopen(output, "w");
       if (op == NULL) {
           printf("Cannot open %s\n", output);
           goto oops;
       }

       wide = 0;
       for (f = curpos; f < endpos; f++) {
           ch = fgetc(fp);

               if (ch == 0 || ch == 10 || ch == 13) {
                   wide = 0;
                   fputc(10, op);
                   continue;
               }

               if (ch == 9) {
                   fputc(9, op);
                   wide++;
                   continue;
               }

               if (ch > 31) {
                   if (wide == MAX_WIDE) {
                       fputc(10, op);
                       wide = 0;

                       if (ch == 32) continue;
                   }
                   fputc(ch, op);
                   wide++;
               }

       } /* end for */

oops:  fclose(fp);
       fclose(op);
   } /* end while */

   exit(0);
}


long getline(char *s, long n, FILE *iop)
{
   int c, f;

   for (f = 0; f < n -1 && (c = getc(iop)) != EOF && c; f++)
       s[f] = c;

   s[++f] = '\0';
   if (!c)
       return f;
   else
       return 0;
}


long getbuff(char *s, long n, FILE *iop)
{
   int c, f;

   for (f = 0; f < n -1 && (c = getc(iop)) != EOF; f++)
       s[f] = c;

   s[++f] = '\0';
   return f;
}


void indicate(int chars)
{
   sd_pos(fgetchid(stdout), -1, 5,5);
   printf("%d characters done so far.", chars);
}
