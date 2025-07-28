
/*** STEP 1. BUILD SYM_TABS *************
--- Symbol Table --- $_global_$
(name) binarySearch, (type) function| auto(auto, auto, auto)
(name) initialize, (type) function| auto(auto)
(name) main, (type) function| auto()
--- Symbol Table --- binarySearch
(name) high, (type) auto
(name) low, (type) auto
(name) x, (type) auto
--- Symbol Table --- binarySearch_$1
(name) mid, (type) auto
--- Symbol Table --- binarySearch_$1_1
--- Symbol Table --- binarySearch_$1_2
--- Symbol Table --- initialize
(name) i, (type) int
(name) n, (type) auto
--- Symbol Table --- initialize_$1
--- Symbol Table --- main
(name) n, (type) auto
(name) result, (type) auto
(name) x, (type) int
--- Symbol Table --- main_$1
--- Symbol Table --- main_$2
 ---    end of step 1       ------------*/

/*** STEP 2. ANALYZE TYPES  *************
--- Symbol Table --- $_global_$
(name) binarySearch, (type) function| int(int, int, int)
(name) initialize, (type) function| int(int)
(name) main, (type) function| int()
--- Symbol Table --- binarySearch
(name) high, (type) int
(name) low, (type) int
(name) x, (type) int
--- Symbol Table --- binarySearch_$1
(name) mid, (type) int
--- Symbol Table --- binarySearch_$1_1
--- Symbol Table --- binarySearch_$1_2
--- Symbol Table --- initialize
(name) i, (type) int
(name) n, (type) int
--- Symbol Table --- initialize_$1
--- Symbol Table --- main
(name) n, (type) int
(name) result, (type) int
(name) x, (type) int
--- Symbol Table --- main_$1
--- Symbol Table --- main_$2
 ---    end of step 2       ------------*/

/*** STEP 3. TRANSFORM to C *************/
#include <stdio.h>
#include <stdlib.h>
#include <stdbool.h>
#include "array.h"
int
initialize (int n)
{
  int i = 0;
  while (i < n)
    {
      setArr (i, i + 3);
      i = i + 1;
    }
  return (n);
}

int
binarySearch (int x, int low, int high)
{
  if (high >= low)
    {
      int mid;
      mid = low + (high - low) / 2;
      if (getArr (mid) == x)
	return (mid);
      if (getArr (mid) > x)
	return (binarySearch (x, low, mid - 1));
      return (binarySearch (x, mid + 1, high));
    }
  return (-1);
}

int
main ()
{
  int n;
  int x = 4;
  int result;
  n = 10;
  initialize (n);
  result = binarySearch (x, 0, n - 1);
  if (result == -1)
    printf ("Not found!!\n");

  else
    printf ("Element is found at index %d\n", result);
  return (result);
}
