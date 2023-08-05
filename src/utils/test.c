#include "string.h"
#include "string.c"

#include <string.h>
#include <locale.h>

int main(void) {
  setlocale(LC_ALL, "");

//   u_tlstring str1 = wcsdup(L"first");
//   u_tlstring str2 = wcsdup(L"second");
//   u_tlstring result = u_tlstring_format(L"0 %d %ls abc %f %ls %d 100", 1, str1, 10.0, str2, 2);
//   wprintf(L"%ls\n", result);
//   free(result);

  char str[] = "😄ğ";
  printf("%s %ld\n", str, strlen(str));

  return 0;
}
