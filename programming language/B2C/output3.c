
/*** STEP 1. BUILD SYM_TABS *************
--- Symbol Table --- $_global_$
(name) main, (type) function| auto()
--- Symbol Table --- main
(name) a, (type) auto
(name) b, (type) auto
(name) c, (type) auto
(name) f, (type) auto
(name) g, (type) auto
(name) h, (type) auto
(name) i, (type) auto
(name) j, (type) auto
(name) k, (type) auto
(name) x, (type) auto
(name) y, (type) auto
(name) z, (type) auto
 ---    end of step 1       ------------*/

/*** STEP 2. ANALYZE TYPES  *************
--- Symbol Table --- $_global_$
(name) main, (type) function| int()
--- Symbol Table --- main
(name) a, (type) char
(name) b, (type) char
(name) c, (type) char
(name) f, (type) double
(name) g, (type) double
(name) h, (type) double
(name) i, (type) int
(name) j, (type) int
(name) k, (type) int
(name) x, (type) bool
(name) y, (type) bool
(name) z, (type) bool
 ---    end of step 2       ------------*/

/*** STEP 3. TRANSFORM to C *************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
int
main ()
{
  int i;
  int j;
  int k;
  char a;
  char b;
  char c;
  double f;
  double g;
  double h;
  bool x;
  bool y;
  bool z;
  i = 10;
  j = 20;
  a = 'A';
  b = 'a';
  f = 3.14;
  g = 2.718;
  x = true;
  y = false;
  k = j / i;
  c = 'T' + (b - a);
  h = f * g;
  z = x && y;
  printf ("k = %d,  c = '%c',  h = %lf,  z = %s\n", k, c, h,
	  z == true ? "true" : "false");
  return (0);
}
