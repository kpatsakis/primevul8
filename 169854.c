copyStringArray(char **array) {
	int len;
	char **copy;
	if (!array) return NULL;
	len = 0;
	while (array[len]) len++;
	copy = malloc((len + 1) * sizeof(char *));
	copy[len] = NULL;
	while (len) {
		len--;
		copy[len] = strdup(array[len]);
	}
	return copy;
}