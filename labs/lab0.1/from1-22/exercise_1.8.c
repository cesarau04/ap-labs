#include<stdio.h>

int main() {
    int c, newlines, tabs, spaces;
    newlines = 0;
    tabs = 0;
    spaces = 0;
    while ((c = getchar()) != EOF) {
        if (c == '\n')
            ++newlines;
        if (c == '\t')
            ++tabs;
        if (c == ' ')
            ++spaces;
    }
    printf("Blanks: %d\nTabs: %d\nNewlines: %d\n", newlines, tabs, spaces);
}
