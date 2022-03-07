#include "str.h"
#include "include/util/util.h"
#include <stdarg.h>
#include <stdbool.h>
#include <stdint.h>
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

bool str_p_equalsvv(view l, view r) {
  if (l.len != r.len)
    return false;
  if (l.data == r.data)
    return true;

  for (size_t i = 0; i < l.len; ++i) {
    if (l.data[i] != r.data[i]) {
      return false;
    }
  }

  return true;
}

bool str_p_matchvva(view input, view pattern, ...) {
  va_list args;
  va_start(args, pattern);
  for (size_t i = 0; i < pattern.len;) {
    view wildcard = str_view("{}");

    // first - find the first character after {} in pattern string
    size_t wildcard_pos = str_find(pattern, wildcard, 0);
    view match = str_sub(pattern, wildcard_pos + wildcard.len, 1);

    // now - find same character at or after {} position
    // thus we get the wildcard's match
    size_t match_pos = str_find(input, match, wildcard_pos);
    view slice = str_sub(input, wildcard_pos, match_pos - wildcard_pos);

    if (slice.len == 0) {
      break;
    }

    *va_arg(args, view *) = slice;

    // "consume" used pattern/input data
    size_t sub_start = wildcard_pos + wildcard.len;
    pattern = str_sub(pattern, sub_start, pattern.len - sub_start);
    size_t rest_start = wildcard_pos + slice.len;
    input = str_sub(input, rest_start, input.len - rest_start);
  }
  va_end(args);
  return true;
}

size_t str_abs_idx(view v, intmax_t idx) {
  if (idx >= 0) {
    return idx;
  } else {
    return v.len + idx;
  }
}
