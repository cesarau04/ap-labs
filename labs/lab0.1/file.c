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
