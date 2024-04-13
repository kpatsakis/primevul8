dump_buffer(char *buff, size_t count, FILE* fp, int indent)
{
	size_t i, j, c;
	bool printnext = true;

	if (count % 16)
		c = count + (16 - count % 16);
	else
		c = count;

	for (i = 0; i < c; i++) {
		if (printnext) {
			printnext = false;
			fprintf(fp, "%*s%.4zu ", indent, "", i & 0xffff);
		}
		if (i < count)
			fprintf(fp, "%3.2x", buff[i] & 0xff);
		else
			fprintf(fp, "   ");
		if (!((i + 1) % 8)) {
			if ((i + 1) % 16)
				fprintf(fp, " -");
			else {
				fprintf(fp, "   ");
				for (j = i - 15; j <= i; j++)
					if (j < count) {
						if ((buff[j] & 0xff) >= 0x20
						    && (buff[j] & 0xff) <= 0x7e)
							fprintf(fp, "%c",
							       buff[j] & 0xff);
						else
							fprintf(fp, ".");
					} else
						fprintf(fp, " ");
				fprintf(fp, "\n");
				printnext = true;
			}
		}
	}
}