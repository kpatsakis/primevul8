static int pdf_findobj(struct pdf_struct *pdf)
{
    const char *start, *q, *q2, *q3, *eof;
    struct pdf_obj *obj;
    off_t bytesleft;
    unsigned genid, objid;

    pdf->nobjs++;
    pdf->objs = cli_realloc2(pdf->objs, sizeof(*pdf->objs)*pdf->nobjs);
    if (!pdf->objs) {
	cli_warnmsg("cli_pdf: out of memory parsing objects (%u)\n", pdf->nobjs);
	return -1;
    }
    obj = &pdf->objs[pdf->nobjs-1];
    memset(obj, 0, sizeof(*obj));
    start = pdf->map+pdf->offset;
    bytesleft = pdf->size - pdf->offset;
    q2 = cli_memstr(start, bytesleft, " obj", 4);
    if (!q2)
	return 0;/* no more objs */
    bytesleft -= q2 - start;
    q = findNextNonWSBack(q2-1, start);
    while (q > start && isdigit(*q)) { q--; }
    genid = atoi(q);
    q = findNextNonWSBack(q-1,start);
    while (q > start && isdigit(*q)) { q--; }
    objid = atoi(q);
    obj->id = (objid << 8) | (genid&0xff);
    obj->start = q2+4 - pdf->map;
    obj->flags = 0;
    bytesleft -= 4;
    eof = pdf->map + pdf->size;
    q = pdf->map + obj->start;
    while (q < eof && bytesleft > 0) {
	off_t p_stream, p_endstream;
	q2 = pdf_nextobject(q, bytesleft);
	if (!q2)
	    return 0;/* no more objs */
	bytesleft -= q2 - q;
	if (find_stream_bounds(q-1, q2-q, bytesleft + (q2-q), &p_stream, &p_endstream)) {
	    obj->flags |= 1 << OBJ_STREAM;
	    q2 = q-1 + p_endstream + 6;
	    bytesleft -= q2 - q + 1;
	} else if ((q3 = cli_memstr(q-1, q2-q+1, "endobj", 6))) {
	    q2 = q3 + 6;
	    pdf->offset = q2 - pdf->map;
	    return 1; /* obj found and offset positioned */
	} else {
	    q2++;
	}
	q = q2;
    }
    return 0;/* no more objs */
}