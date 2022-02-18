#include "str.h"
#include "include/util/util.h"
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

str str_p_ownv(view v) {
  size_t bufsz = v.len * sizeof(*v.data);
  void *copy = malloc(bufsz);
  memcpy(copy, v.data, bufsz);

  return (str){
      .len = v.len,
      .data = copy,
  };
}

void str_p_frees(str s) { free((void *)s.data); }

view str_p_viewcp8(char const *c) {
  return (view){
      .len = strlen(c),
      .data = c,
  };
}

view str_p_views(str s) {
  return (view){
      .len = s.len,
      .data = s.data,
  };
}

view str_p_viewv(view v) { return v; }

view str_p_viewi(int c) {
  char const *printables = " !\"#$%&\'()*+,-./"
                           "0123456789:;<=>?@ABCDEFGHIJKLMNOPQRSTUVWXYZ[\\]^_`"
                           "abcdefghijklmnopqrstuvwxyz{|}~";
  return (view){
      .len = 1,
      .data = &printables[c - *printables],
  };
}

str str_p_concatva(size_t num_args, ...) {
  va_list args;
  size_t len = 0;
  wrap(va_start(args, num_args), va_end(args)) {
    for (size_t i = 0; i < num_args; ++i) {
      len += va_arg(args, view).len;
    }
  }

  char *data = malloc(len * sizeof(*dummy(view)->data));
  wrap(va_start(args, num_args), va_end(args)) {
    for (size_t i = 0, running_len = 0; i < num_args; ++i) {
      view e = va_arg(args, view);
      memcpy(&data[running_len], e.data, e.len * sizeof(*e.data));
      running_len += e.len;
    }
  }

  return (str){
      .len = len,
      .data = data,
  };
}

view str_p_subvsl(view v, size_t start, size_t len) {
  return (view){
      .len = len,
      .data = &v.data[start],
  };
}

void str_p_printv(view v) {
  for (size_t i = 0; i < v.len; ++i) {
    putchar(v.data[i]);
  }
}

size_t str_p_findvv(view v, view target, size_t start) {
  for (size_t i = start; i < v.len; ++i) {
    if ((v.len - i) < target.len) {
      break;
    }

    for (size_t j = 0; j < target.len; ++j) {
      if (v.data[i + j] != target.data[j]) {
        goto next;
      }
    }
    return i;
  next:;
  }
  return v.len;
}

int main() {
  view v = str_view("my@@name@@is@@slim@@shady");
  view f = str_view("@@");

  str_split_for(v, f) {
    view it = str_split_it(v);
    str_println(it);
  }

  return 0;
}
