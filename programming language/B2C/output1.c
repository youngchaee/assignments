
/*** STEP 1. BUILD SYM_TABS *************
--- Symbol Table --- $_global_$
(name) main, (type) function| auto()
--- Symbol Table --- main
(name) i, (type) auto
(name) last, (type) int
(name) s, (type) auto
(name) x, (type) auto
--- Symbol Table --- main_$1
 ---    end of step 1       ------------*/

/*** STEP 2. ANALYZE TYPES  *************
--- Symbol Table --- $_global_$
(name) main, (type) function| double()
--- Symbol Table --- main
(name) i, (type) int
(name) last, (type) int
(name) s, (type) double
(name) x, (type) bool
--- Symbol Table --- main_$1
 ---    end of step 2       ------------*/

/*** STEP 3. TRANSFORM to C *************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include <string>
#define string std::string
double
main ()
{
  double s;
  int i;
  bool x;
  int last = 100;
  s = 0.0;
  i = 1;
  x = true;
  while (x == true)
    {
      s = s + s;
      i = i + 1;
      x = i < last;
    }
  printf ("sum(%d) = %f\n", last, s);
  return (s);
}
