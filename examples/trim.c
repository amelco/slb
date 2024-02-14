#include <stdio.h>
#include <string.h>


void trim(char *str) {
  char *start =
      str +
      strspn(str, " \t\n\r");  // Points to the first non-whitespace character
  char *end =
      str + strlen(str) - 1;  // Points to the last character of the string

  while (end > start &&
         (*end == ' ' || *end == '\t' || *end == '\n' || *end == '\r')) {
    --end;  // Move the end pointer backward while it points to whitespace
  }

  *(end + 1) = '\0';  // Place the null terminator after the last non-whitespace
                      // character

  // Move the trimmed string to the start of the buffer
  if (start > str) {
    memmove(str, start,
            end - start +
                2);  // +2 to include the last character and null terminator
  }
}

int main(void)
{
    char *s = "  abc  def  ";
    printf("Before trim: %s\n", s);
    printf("len: %zu\n", strlen(s));
    trim(s);
    printf("After trim: %s\n", s);
    printf("len: %zu\n", strlen(s));

    return 0;
}
