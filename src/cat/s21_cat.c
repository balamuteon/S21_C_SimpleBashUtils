#include <getopt.h>  // Include the getopt header
#include <stdio.h>
#include <string.h>

#define SHORT_OPTIONS "beEnstT"
#define SHORT_OPTIONS_LENGTH 7

typedef struct {
  char b, e, n, s, t, v;
} options;

void scan_options(int argc, char **argv, options *opt,
                  struct option long_options[]);
void process_file(const char *file_name_argument, const options *opt);
void read_and_write(FILE *file_stream, const options *opt);
void print_num(int *line_num);
void set_shift(int *ch);

int main(int argc, char *argv[]) {
  options opt = {0};
  struct option long_options[] = {{"number", no_argument, NULL, 'n'},
                                  {"number-nonblank", no_argument, NULL, 'b'},
                                  {"squeeze-blank", no_argument, NULL, 's'},
                                  {"show-nonprinting", no_argument, NULL, 'v'},
                                  {"show-ends", no_argument, NULL, 'e'},
                                  {"show-tabs", no_argument, NULL, 't'},
                                  {"show-tabs", no_argument, NULL, 'T'},
                                  {0, 0, 0, 0}};
  if (argc > 1) {
    scan_options(argc, argv, &opt, long_options);

    for (int i = optind; i < argc; i++) process_file(argv[i], &opt);
  } else
    fprintf(stderr, "usage: program_name [-beEnstT] file_name1 ...\n");

  return 0;
}

void scan_options(int argc, char **argv, options *opt,
                  struct option long_options[]) {
  int c;

  while ((c = getopt_long(argc, argv, SHORT_OPTIONS, long_options, 0)) != -1) {
    switch (c) {
      case 'b':
        opt->b = 1;
        break;
      case 'e':
        opt->e = 1;
        opt->v = 1;
        break;
      case 'n':
        opt->n = opt->b ? 0 : 1;
        break;
      case 's':
        opt->s = 1;
        break;
      case 't':
        opt->t = 1;
        opt->v = 1;
        break;
      case 'T':
        opt->t = 1;
        break;
      case '?':
        fprintf(stderr, "Invalid option\n");
        break;
    }
  }
}

void process_file(const char *file_name_argument, const options *opt) {
  FILE *file_stream = fopen(file_name_argument, "r");
  if (file_stream != NULL) {
    read_and_write(file_stream, opt);
    fclose(file_stream);
  } else
    fprintf(stderr, "Error: Unable to open file '%s'\n", file_name_argument);
}

void read_and_write(FILE *file_stream, const options *opt) {
  int ch;
  static int line_number = 1;
  int is_new_line = 1;
  int blank_line_count = 0;

  while ((ch = fgetc(file_stream)) != EOF) {
    if (opt->s) {
      blank_line_count = (is_new_line && ch == '\n') ? blank_line_count + 1 : 0;
      if (blank_line_count > 1) continue;
    }

    if (opt->n && is_new_line) print_num(&line_number);

    if (opt->b && is_new_line && ch != '\n') print_num(&line_number);

    if (opt->t && ch == '\t') {
      printf("^I");
      continue;
    }

    if (opt->e && ch == '\n') putchar('$');

    if (opt->v && ch != '\r' && ch != '\n' && ch != '\t') {
      set_shift(&ch);
    }

    putchar(ch);
    is_new_line = (ch == '\n');
  }
}

void print_num(int *line_number) {
  printf("%6d\t", *line_number);
  *line_number += 1;
}

void set_shift(int *ch) {
  if (*ch < 32) {
    *ch = *ch + 64;
    putchar('^');
  } else if (*ch > 127) {
    putchar('M');
    putchar('-');
    *ch = *ch - 128;
  }
  if (*ch == 127) {
    *ch = *ch - 64;
    putchar('^');
  }
}