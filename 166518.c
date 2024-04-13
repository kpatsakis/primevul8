_PUBLIC_ void file_lines_slashcont(char **lines)
{
	int i, j;

	for (i=0; lines[i];) {
		int len = strlen(lines[i]);
		if (lines[i][len-1] == '\\') {
			lines[i][len-1] = ' ';
			if (lines[i+1]) {
				char *p = &lines[i][len];
				while (p < lines[i+1]) *p++ = ' ';
				for (j = i+1; lines[j]; j++) lines[j] = lines[j+1];
			}
		} else {
			i++;
		}
	}
}