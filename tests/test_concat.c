#include <stdio.h>
#include "ustr.h"

int main() {
    UStr s1 = new_ustr("Hello ");
    UStr s2 = new_ustr("World!");
    UStr result = concat(s1, s2);

    print_ustr(result);

    free_ustr(s1);
    free_ustr(s2);
    free_ustr(result);

    return 0;
}
