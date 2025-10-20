#include <ctype.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define SHORT_OPTIONS "eivclnhso"
#define SHORT_OPTIONS_LENGTH 9
#define SET_FLAG(flag) opt->flag = 1;

typedef struct {
  int error;
  int template_is_not_provided;
  int template_index;
  int arg_index;
} sup_var;

typedef struct {
  char e, i, v, c, l, n, h, s, o;
} options;

int is_flag(const char *argv) { return argv[0] == '-'; }

int scan_options(options *opt, const char *argv) {
  int opt_found = 1;
  if (argv[2] != '\0') opt_found = 0;
  if (argv[1] == 'e') {
    SET_FLAG(e);
  } else if (argv[1] == 'i') {
    SET_FLAG(i);
  } else if (argv[1] == 'v') {
    SET_FLAG(v);
  } else if (argv[1] == 'c') {
    SET_FLAG(c);
  } else if (argv[1] == 'l') {
    SET_FLAG(l);
  } else if (argv[1] == 'n') {
    SET_FLAG(n);
  } else if (argv[1] == 'h') {
    SET_FLAG(h);
  } else if (argv[1] == 's') {
    SET_FLAG(s);
  } else if (argv[1] == 'o') {
    SET_FLAG(o);
  } else {
    opt_found = 0;
  }

  return opt_found ? argv[1] : 0;
}

void print_file_name(char *file_name) { printf("%s:", file_name); }

void print_string(char *string_to_print) { printf("%s\n", string_to_print); }

void print_str_number(int number) { printf("%d:", number); }

void copy_tolower(int size, char src[size], char dest[size]) {
  int i = 0;
  for (; i < size; i++) dest[i] = tolower(src[i]);
}

int string_is_matched(char *target_string, regex_t *template) {
  return regexec(template, target_string, 0, NULL, 0) == 0;
}

void ignore_register(char *buffer, regex_t *template) {
  size_t buffer_size = strlen(buffer) + 1;
  char *lower_buffer = malloc(buffer_size);
  copy_tolower(buffer_size, buffer, lower_buffer);
  if (string_is_matched(lower_buffer, template)) print_string(buffer);

  free(lower_buffer);
}

void handle_matched_parts(const char *buffer, regex_t *template) {
  regmatch_t matches[1];
  const char *str = buffer;

  while (regexec(template, str, 1, matches, 0) == 0) {
    int start = matches[0].rm_so;
    int end = matches[0].rm_eo;

    printf("%.*s\n", end - start, str + start);

    str += end;
  }
}

void remove_new_line_character(char *string) {
  size_t len = strlen(string);
  if (len > 0 && string[len - 1] == '\n') string[len - 1] = '\0';
}

void process_file(FILE *file_stream, regex_t *template, options *opt,
                  sup_var *v, int argc, char *file_name) {
  char buffer[1024];
  int file_string_counter = 1;
  int file_is_found = 0;
  int amount_of_files = argc - (v->template_index + 1);
  int match_counter = 0;
  while (fgets(buffer, sizeof(buffer), file_stream) != NULL &&
         file_is_found == 0) {
    remove_new_line_character(buffer);
    int match = string_is_matched(buffer, template);
    if (amount_of_files > 1 && !opt->h && match && !opt->c && !opt->v &&
        !opt->l)
      print_file_name(file_name);
    if (opt->o)
      handle_matched_parts(buffer, template);
    else if (opt->i)
      ignore_register(buffer, template);
    else if (opt->v) {
      if (!match) {
        if (amount_of_files > 1) {
          print_file_name(file_name);
        }
        print_string(buffer);
      }
    } else if (opt->c) {
      if (match) match_counter++;
    } else if (opt->l) {
      if (match) {
        file_is_found = 1;
      }
    } else if (opt->n) {
      if (match) {
        print_str_number(file_string_counter);
        print_string(buffer);
      }
    } else if (opt->h) {
      if (match) print_string(buffer);
    } else {
      if (match) {
        print_string(buffer);
      }
    }
    file_string_counter++;
  }
  if (opt->c) {
    if (amount_of_files > 1) print_file_name(file_name);
    printf("%d\n", match_counter);
  }
  if (file_is_found) printf("%s\n", file_name);
}

void read_file(char *prog_name, char *file_name, int argc, regex_t *template,
               options *opt, sup_var *v) {
  FILE *file_stream = fopen(file_name, "r");

  if (file_stream != NULL) {
    process_file(file_stream, template, opt, v, argc, file_name);
    fclose(file_stream);
  } else {
    if (!opt->s)
      fprintf(stderr, "%s: %s: No such file or directory\n", prog_name,
              file_name);
  }
}

void sort_arguments(int argc, char **argv, options *opt, sup_var *v,
                    regex_t *template, char *temp_string) {
  for (int i = 1; i < argc && v->error == 0; i++) {
    int flag = is_flag(argv[i]);
    v->arg_index = i;
    if (flag) {
      if ((scan_options(opt, argv[i])) == 0) {
        printf("Invalid option");
        v->error = 1;
      }
      v->template_is_not_provided = 1;
    } else if ((!flag || opt->e) && v->template_is_not_provided) {
      strcpy(temp_string, argv[i]);
      v->template_is_not_provided = 0;
      v->template_index = i;
    } else {
      int retu = regcomp(template, temp_string,
                         opt->i ? REG_EXTENDED | REG_ICASE : REG_EXTENDED);
      if (retu != 0) {
        v->error = 1;
        printf("%s", "error");
      }
      read_file(argv[0], argv[i], argc, template, opt, v);
    }
  }
}

int main(int argc, char *argv[]) {
  options opt = {0};
  sup_var v = {0, 1, 0, 0};
  regex_t template;
  char temp_string[256];
  temp_string[0] = '\0';

  if (argc > 2) {
    sort_arguments(argc, argv, &opt, &v, &template, temp_string);
    if (!v.error) regfree(&template);
  } else
    printf("No arguments provided");

  return 0;
}