#ifndef STR_H_
#define STR_H_

#include <stddef.h>

struct str {
  size_t len;
  char const *data;
};

struct view {
  size_t len;
  char const *data;
};

typedef struct str str;
typedef struct view view;

#define no_op_ (void)0

// view: creates a read-only view of the string
view str_p_viewcp8(char const *);
view str_p_views(str);
view str_p_viewv(view);
view str_p_viewi(int);
#define str_view(s)                                                            \
  _Generic((s), char * : str_p_viewcp8, str : str_p_views, view : str_p_viewv, int : str_p_viewi)(s)

// own: creates a copy of the given string
str str_p_ownv(view);
#define str_own(s) str_p_ownv(view(s))

// strfree: frees the resources of the given string
void str_p_frees(str);
#define str_free(s) _Generic((s), str : str_p_frees(s), view : no_op_)

// concat: joins two strings into an owned string
str str_p_concatva(size_t, ...);
//#define str_concat(x, y) concatvv(view(x), view(y))

// sub: creates a view from the given range
// has two variants:
// sub(view, start, len): substrings view in the range [start, len)
// sub(view, len): substrings view in the range [0, len)
view str_p_subvsl(view, size_t start, size_t len);
#define str_p_subl(s, len) str_p_subsl((s), 0, (len))
#define str_p_subsl(s, start, end) str_p_subvsl(str_view((s)), (start), (end))
#define str_p_sub_choose(_1, _2, f, ...) f
#define str_sub(s, ...)                                                        \
  str_p_sub_choose(__VA_ARGS__, str_p_subsl, str_p_subl,                       \
                   bad_number_of_arguments)(s, __VA_ARGS__)

// print: print a string (no new line)
void str_p_printv(view);
#define str_print(s) str_p_printv(str_view(s))

// println: print a string (with new line)
#define str_println(s)                                                         \
  str_print(s);                                                                \
  str_print("\n")

// find: find and return the index of the first occurence of a view in the given
// view, skipping `start` characters
size_t str_p_findvv(view, view target, size_t start);
#define str_find(v, f, s) str_p_findvv(str_view(v), str_view(f), (s))

// str_split_for: used for iterating on a string, splitting it into parts
// correct usage is:
// str_split_for(target, split_by) {
//   view it = str_split_it(target);
//   /* do something with `it` */
// }
#define str_split_for(v, f)                                                    \
  for (size_t _p_temp, _p_start = 0, _p_end = str_find(v, f, _p_start);        \
       _p_start < v.len;                                                       \
       _p_temp = _p_end, _p_end = str_find(v, f, _p_end + f.len),              \
                       _p_start = _p_temp + f.len)
#define str_split_it(v) str_sub(v, _p_start, _p_end - _p_start)

#endif // STR_H_
