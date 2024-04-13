ldns_fskipcs_l(FILE *fp, const char *s, int *line_nr)
{
        bool found;
        int c;
        const char *d;

	while ((c = fgetc(fp)) != EOF) {
		if (line_nr && c == '\n') {
			*line_nr = *line_nr + 1;
		}
                found = false;
                for (d = s; *d; d++) {
                        if (*d == c) {
                                found = true;
                        }
                }
		if (!found) {
			/* with getc, we've read too far */
			ungetc(c, fp);
			return;
		}
	}
}