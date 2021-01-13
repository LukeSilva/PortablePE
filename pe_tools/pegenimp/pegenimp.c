#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

const char* header = 
  ".section .win_imports\n"
  "image_base = 0x00400000\n\n"
  "windows_imports:\n"
  "\n"
  "  # Header for create_exe\n"
  "  .byte 'P', 'E', 0x00, 0x00\n"
  "  # Windows entry point\n"
  "  .long _start_win\n";

const char* null_dll_import = 
  "\n  /* NULL Entry */\n"
  "  .long 0\n" // INT
  "  .long 0\n" // Date time stamp
  "  .long 0\n" // Forwarder chain
  "  .long 0\n" // DLL Name
  "  .long 0\n"; // IAT

struct import {
  char* name;
  struct import* nxt;
};

struct dll {
  char* name;
  struct import * imports;
  struct dll* nxt;
};

void traverse(
  struct dll * list, 
  void (*start_dll)(struct dll* cur, int i),
  void (*process_import)(struct dll* dll, int dll_id, struct import * import),
  void (*end_dll)(struct dll* cur, int i)
);

char line[256];
int indent;
int len;
FILE* out;

void read_line(FILE* f) {
  const char * str = fgets(line, 255, f);
  if (str == NULL) {
    line[0] = 0;
    indent = 0;
    len = 0;
    return;
  }

  int i = 0;
  while(isspace(line[i])) { i += 1; }; // Get indentation

  // If the line is empty or a comment, read next line
  if (line[i] == 0 || line[i] == '#') return read_line(f);
  
  indent = i;

  // Remove trailing whitespace
  while(line[++i]!=0);
  while(isspace(line[--i])) { line[i] = 0;}
  len = i + 1;

  //printf("read_line (%2d,%4d): '%s'\n", indent, len, line);
}

struct import * read_import(FILE* f) {
  read_line(f);

  if (indent == 0) return NULL;

  struct import * import = (struct import *) malloc(sizeof(import));
  import->name = (char*) malloc(len - indent);
  strcpy(import->name, line + indent);

  return import;
}

struct dll * read_dll(FILE* f) {

  if (indent != 0) {
    printf("read_dll: expected indent of 0");
    return NULL;
  }

  if (!len) return NULL;

  struct dll * dll = (struct dll *) malloc(sizeof(dll));
  dll->imports = NULL;
  dll->name = (char*)malloc(len);
  strcpy(dll->name, line);

  struct import ** nxt = &dll->imports;

  while (*nxt = read_import(f)) {nxt = &(*nxt)->nxt;}

  return dll;
}

void print_import_table(struct dll * dll, int i) {
  fprintf(out,"\n  /* DLL %s */\n", dll->name);
  fprintf(out,"  .long _dll_%d_int - image_base\n", i); // INT
  fprintf(out,"  .long 0\n"); // Date time stamp
  fprintf(out,"  .long 0\n"); // Forwarder chain
  fprintf(out,"  .long _dll_%d_name - image_base\n", i); // DLL Name
  fprintf(out,"  .long _dll_%d_iat - image_base\n", i); // IAT
}

void print_dll_name(struct dll* dll, int i) {
  fprintf(out,"\n");
  fprintf(out,"_dll_%d_name:\n", i);
  fprintf(out,"  .asciz \"%s\"\n", dll->name);
  fprintf(out,"  .align 2\n");
}



void print_int_head(struct dll* dll, int i) {
  fprintf(out,"\n");
  fprintf(out,".align 4\n");
  fprintf(out,"_dll_%d_int:\n", i);
}
void print_int_import(struct dll* dll, int dll_id, struct import * import) {
  fprintf(out,"  .long _dll_%d_%s - image_base\n", dll_id, import->name);
}

void print_int_tail(struct dll* dll, int i) {
  fprintf(out,"  .long 0\n");
}

void print_int(struct dll* list) {
  traverse(list, print_int_head, print_int_import, print_int_tail);
}



void print_iat_head(struct dll* dll, int i) {
  fprintf(out,"\n");
  fprintf(out,".align 4\n");
  fprintf(out,"_dll_%d_iat:\n", i);
}
void print_iat_import(struct dll* dll, int dll_id, struct import * import) {
  fprintf(out,"  WIN32_%s:\n", import->name);
  fprintf(out,"    .long _dll_%d_%s - image_base\n", dll_id, import->name);
}

void print_iat_tail(struct dll* dll, int i) {
  fprintf(out,"  .long 0\n");
}

void print_iat(struct dll* list) {
  traverse(list, print_iat_head, print_iat_import, print_iat_tail);
}



void _print_global_imports(struct dll* dll, int dll_id, struct import * import) {
  fprintf(out,".global WIN32_%s\n", import->name);
}

void print_global_imports(struct dll* list) {
  traverse(list, NULL, _print_global_imports, NULL);
}



void _print_name_hint(struct dll* dll, int dll_id, struct import * import) {
  fprintf(out,"\n_dll_%d_%s:\n", dll_id, import->name);
  fprintf(out,"  .short 0\n"); // Hint (0 cause I can't be stuffed)
  fprintf(out,"  .asciz \"%s\"\n", import->name);
  fprintf(out,"  .align 2\n");
}

void print_name_hint(struct dll* list) {
  traverse(list, NULL, _print_name_hint, NULL);
}



void traverse(
  struct dll * list, 
  void (*start_dll)(struct dll* cur, int i),
  void (*process_import)(struct dll* dll, int dll_id, struct import * import),
  void (*end_dll)(struct dll* cur, int i)
) {
  int i = 1;
  struct dll * cur_dll = list;
  while (cur_dll != NULL) {
    if (start_dll != NULL)
      start_dll(cur_dll, i);
    
    if (process_import != NULL) {
      struct import * cur_import = cur_dll->imports;
      while (cur_import != NULL) {
        process_import(cur_dll, i, cur_import);
        cur_import = cur_import->nxt;
      }
    }

    if (end_dll != NULL)
      end_dll(cur_dll, i);
    
    cur_dll = cur_dll ->nxt;
    ++i;
  }
}

void foreach_dll(struct dll * cur, void (*f)(struct dll* cur, int i)) {
  traverse(cur, f, NULL, NULL);
}

int main() {
  FILE * f = fopen("win_imports", "r");
  out = fopen("win_imports.s", "w");

  struct dll * start = NULL;
  struct dll ** nxt = &start;

  read_line(f);
  while (*nxt = read_dll(f)) { 
    nxt = &(*nxt)->nxt;
  }


  print_global_imports(start);
  fprintf(out,"\n");

  fputs(header, out);


  foreach_dll(start, print_import_table);
  fputs(null_dll_import, out);

  print_int(start);
  print_iat(start);

  fprintf(out,".align 2\n");
  print_name_hint(start);
  foreach_dll(start, print_dll_name);



}