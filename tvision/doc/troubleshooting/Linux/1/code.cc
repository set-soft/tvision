#include <stdio.h>

#define ENTER_UTF8  "\e%G"
#define EXIT_UTF8   "\e%@"

int main(int argc, char *argv[])
{
 fputs(EXIT_UTF8,stdout);
 return 0;
}
