static int find_length(struct pdf_struct *pdf,
		       struct pdf_obj *obj,
		       const char *start, off_t len)
{
    int length;
    const char *q;
    q = cli_memstr(start, len, "/Length", 7);
    if (!q)
	return 0;
    q++;
    len -= q - start;
    start = pdf_nextobject(q, len);
    if (!start)
	return 0;
    /* len -= start - q; */
    q = start;
    length = atoi(q);
    while (isdigit(*q)) q++;
    if (*q == ' ') {
	int genid;
	q++;
	genid = atoi(q);
	while(isdigit(*q)) q++;
	if (q[0] == ' ' && q[1] == 'R') {
	    cli_dbgmsg("cli_pdf: length is in indirect object %u %u\n", length, genid);
	    obj = find_obj(pdf, obj, (length << 8) | (genid&0xff));
	    if (!obj) {
		cli_dbgmsg("cli_pdf: indirect object not found\n");
		return 0;
	    }
	    q = pdf_nextobject(pdf->map+obj->start, pdf->size - obj->start);
	    length = atoi(q);
	}
    }
    /* limit length */
    if (start - pdf->map + length+5 > pdf->size) {
	length = pdf->size - (start - pdf->map)-5;
    }
    return length;
}