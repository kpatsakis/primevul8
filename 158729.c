static int pdf_extract_obj(struct pdf_struct *pdf, struct pdf_obj *obj)
{
    char fullname[NAME_MAX + 1];
    int fout;
    off_t sum = 0;
    int rc = CL_SUCCESS;
    char *ascii_decoded = NULL;

    if (!(obj->flags & DUMP_MASK)) {
	/* don't dump all streams */
	return CL_CLEAN;
    }
    snprintf(fullname, sizeof(fullname), "%s"PATHSEP"pdf%02u", pdf->dir, pdf->files++);
    fout = open(fullname,O_RDWR|O_CREAT|O_EXCL|O_TRUNC|O_BINARY, 0600);
    if (fout < 0) {
	char err[128];
	cli_errmsg("cli_pdf: can't create temporary file %s: %s\n", fullname, cli_strerror(errno, err, sizeof(err)));
	free(ascii_decoded);
	return CL_ETMPFILE;
    }

    do {
    if (obj->flags & (1 << OBJ_STREAM)) {
	const char *start = pdf->map + obj->start;
	off_t p_stream = 0, p_endstream = 0;
	off_t length;
	find_stream_bounds(start, pdf->size - obj->start,
			   pdf->size - obj->start,
			   &p_stream, &p_endstream);
	if (p_stream && p_endstream) {
	    int rc2;
	    const char *flate_in;
	    long ascii_decoded_size = 0;
	    size_t size = p_endstream - p_stream;

	    length = find_length(pdf, obj, start, p_stream);
	    if (!(obj->flags & (1 << OBJ_FILTER_FLATE)) && !length) {
		const char *q = start + p_endstream;
		length = size;
		q--;
		if (*q == '\n') {
		    q--;
		    length--;
		    if (*q == '\r')
			length--;
		} else if (*q == '\r') {
		    length--;
		}
		cli_dbgmsg("cli_pdf: calculated length %ld\n", length);
	    }
	    if (!length)
		length = size;

	    if (obj->flags & (1 << OBJ_FILTER_AH)) {
		ascii_decoded = cli_malloc(length/2 + 1);
		if (!ascii_decoded) {
		    cli_errmsg("Cannot allocate memory for asciidecode\n");
		    rc = CL_EMEM;
		    break;
		}
		ascii_decoded_size = asciihexdecode(start + p_stream,
						    length,
						    ascii_decoded);
	    } else if (obj->flags & (1 << OBJ_FILTER_A85)) {
		ascii_decoded = cli_malloc(length*5);
		if (!ascii_decoded) {
		    cli_errmsg("Cannot allocate memory for asciidecode\n");
		    rc = CL_EMEM;
		    break;
		}
		ascii_decoded_size = ascii85decode(start+p_stream,
						   length,
						   (unsigned char*)ascii_decoded);
	    }
	    if (ascii_decoded_size < 0) {
		pdf->flags |= 1 << BAD_ASCIIDECODE;
		cli_dbgmsg("cli_pdf: failed to asciidecode in %u %u obj\n", obj->id>>8,obj->id&0xff);
		rc = CL_CLEAN;
		break;
	    }
	    /* either direct or ascii-decoded input */
	    if (!ascii_decoded)
		ascii_decoded_size = length;
	    flate_in = ascii_decoded ? ascii_decoded : start+p_stream;

	    if (obj->flags & (1 << OBJ_FILTER_FLATE)) {
		rc = filter_flatedecode(pdf, obj, flate_in, ascii_decoded_size, fout, &sum);
	    } else {
		if (filter_writen(pdf, obj, fout, flate_in, ascii_decoded_size, &sum) != ascii_decoded_size)
		    rc = CL_EWRITE;
	    }
	    cli_updatelimits(pdf->ctx, sum);
	    /* TODO: invoke bytecode on this pdf obj with metainformation associated
	     * */
	    cli_dbgmsg("cli_pdf: extracted %ld bytes %u %u obj to %s\n", sum, obj->id>>8, obj->id&0xff, fullname);
	    lseek(fout, 0, SEEK_SET);
	    rc2 = cli_magic_scandesc(fout, pdf->ctx);
	    if (rc2 == CL_VIRUS || rc == CL_SUCCESS)
		rc = rc2;
	}
    } else if (obj->flags & (1 << OBJ_JAVASCRIPT)) {
	const char *q2;
	const char *q = pdf->map+obj->start;
	/* TODO: get obj-endobj size */
	off_t bytesleft = obj_size(pdf, obj);
	if (bytesleft < 0)
	    break;

	q2 = cli_memstr(q, bytesleft, "/JavaScript", 11);
	if (!q2)
	    break;
	q2++;
	bytesleft -= q2 - q;
	q = pdf_nextobject(q2, bytesleft);
	if (!q)
	    break;
	bytesleft -= q - q2;
	if (*q == '(') {
	    if (filter_writen(pdf, obj, fout, q+1, bytesleft-1, &sum) != (bytesleft-1)) {
		rc = CL_EWRITE;
		break;
	    }
	} else if (*q == '<') {
	    char *decoded;
	    q2 = memchr(q+1, '>', bytesleft);
	    if (!q2) q2 = q + bytesleft;
	    decoded = cli_malloc(q2 - q);
	    if (!decoded) {
		rc = CL_EMEM;
		break;
	    }
	    cli_hex2str_to(q2, decoded, q2-q-1);
	    decoded[q2-q-1] = '\0';
	    cli_dbgmsg("cli_pdf: found hexadecimal encoded javascript in %u %u obj\n",
		       obj->id>>8, obj->id&0xff);
	    pdf->flags |= 1 << HEX_JAVASCRIPT;
	    filter_writen(pdf, obj, fout, decoded, q2-q-1, &sum);
	    free(decoded);
	}
    }
    } while (0);
    close(fout);
    free(ascii_decoded);
    if (!pdf->ctx->engine->keeptmp)
	if (cli_unlink(fullname) && rc != CL_VIRUS)
	    rc = CL_EUNLINK;
    return rc;
}