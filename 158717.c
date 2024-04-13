static void pdf_parseobj(struct pdf_struct *pdf, struct pdf_obj *obj)
{
    /* enough to hold common pdf names, we don't need all the names */
    char pdfname[64];
    const char *q2, *q3;
    const char *q = obj->start + pdf->map;
    const char *dict, *start;
    off_t dict_length;
    off_t bytesleft = pdf->offset - obj->start;
    unsigned i;
    enum objstate objstate = STATE_NONE;

    if (bytesleft < 0)
	return;
    start = q;
    /* find start of dictionary */
    do {
	q2 = pdf_nextobject(q, bytesleft);
	bytesleft -= q2 -q;
	if (!q2 || bytesleft < 0) {
	    return;
	}
	q3 = memchr(q-1, '<', q2-q+1);
	q2++;
	bytesleft--;
	q = q2;
    } while (!q3 || q3[1] != '<');
    dict = q3+2;
    q = dict;
    bytesleft = pdf->offset - obj->start - (q3 - start);
    /* find end of dictionary */
    do {
	q2 = pdf_nextobject(q, bytesleft);
	bytesleft -= q2 -q;
	if (!q2 || bytesleft < 0) {
	    return;
	}
	q3 = memchr(q-1, '>', q2-q+1);
	q2++;
	bytesleft--;
	q = q2;
    } while (!q3 || q3[1] != '>');
    obj->flags |= 1 << OBJ_DICT;
    dict_length = q3 - dict;

    /*  process pdf names */
    for (q = dict;dict_length;) {
	int escapes = 0;
	q2 = memchr(q, '/', dict_length);
	if (!q2)
	    break;
	dict_length -= q2 - q;
	q = q2;
	/* normalize PDF names */
	for (i = 0;dict_length && (i < sizeof(pdfname)-1); i++) {
	    q++;
	    dict_length--;
	    if (*q == '#') {
		cli_hex2str_to(q+1, pdfname+i, 2);
		q += 2;
		dict_length -= 2;
		escapes = 1;
		continue;
	    }
	    if (*q == ' ' || *q == '\r' || *q == '\n' || *q == '/')
		break;
	    pdfname[i] = *q;
	}
	pdfname[i] = '\0';

	handle_pdfname(pdf, obj, pdfname, escapes, &objstate);
	if (objstate == STATE_JAVASCRIPT) {
	    q2 = pdf_nextobject(q, dict_length);
	    if (q2 && isdigit(*q2)) {
		uint32_t objid = atoi(q2) << 8;
		while (isdigit(*q2)) q2++;
		q2 = pdf_nextobject(q2, dict_length);
		if (q2 && isdigit(*q2)) {
		    objid |= atoi(q2) & 0xff;
		    q2 = pdf_nextobject(q2, dict_length);
		    if (*q2 == 'R') {
			struct pdf_obj *obj2;
			cli_dbgmsg("cli_pdf: found javascript stored in indirect object %u %u",
				   objid >> 8, objid&0xff);
			obj2 = find_obj(pdf, obj, objid);
			obj2->flags |= OBJ_JAVASCRIPT;
		    }
		}
	    }
	    objstate = STATE_NONE;
	}
    }
    cli_dbgmsg("cli_pdf: %u %u obj flags: %02x\n", obj->id>>8, obj->id&0xff, obj->flags);
}