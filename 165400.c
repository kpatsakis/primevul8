static int jpc_com_dumpparms(jpc_ms_t *ms, FILE *out)
{
	jpc_com_t *com = &ms->parms.com;
	unsigned int i;
	int printable;
	fprintf(out, "regid = %d;\n", com->regid);
	printable = 1;
	for (i = 0; i < com->len; ++i) {
		if (!isprint(com->data[i])) {
			printable = 0;
			break;
		}
	}
	if (printable) {
		fprintf(out, "data = ");
		fwrite(com->data, sizeof(char), com->len, out);
		fprintf(out, "\n");
	}
	return 0;
}