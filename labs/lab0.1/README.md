## Compile
Use make command
``
make
``
# Usage
```
./exercise_1_23.out [file.c]
```
`file.c` is a dummy file, to test cases only.

# Returns
Prints to console the file without comments.

original:
```
int main(){
        //comment
        int len; //coment
        int max; /* comment 2 */
        char line[11];
        /* char longest[MAXLINE];*/
        char s[] = "// not a comment";
        max = 80;
        while ((len = get_line(line, 11)) > 0){
                printf("%s\n", line);
                memset(line, 0, strlen(line)); /* clear lasttwochars*/
        }
        return 0;
}
```

returned:
```
int main(){

        int len; 
        int max; 
        char line[11];

        char s[] = "// not a comment";
        max = 80;
        while ((len = get_line(line, 11)) > 0){
                printf("%s\n", line);
                memset(line, 0, strlen(line)); 
        }
        return 0;
}
```

# Credits
All examples taken for C The Programming Language book.
