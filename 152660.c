void salloc(int i, char *ltemp)
{
	if ((args[i] = (char *) malloc(strlen(ltemp) + 1)) == NULL) {
		perror("malloc");
		exit(4);
	}
	strcpy(args[i], ltemp);
}