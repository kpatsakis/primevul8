static int filter_flatedecode(struct pdf_struct *pdf, struct pdf_obj *obj,
			      const char *buf, off_t len, int fout, off_t *sum)
{
    int zstat;
    z_stream stream;
    off_t nbytes;
    char output[BUFSIZ];

    if (len == 0)
	return CL_CLEAN;
    memset(&stream, 0, sizeof(stream));
    stream.next_in = (Bytef *)buf;
    stream.avail_in = len;
    stream.next_out = (Bytef *)output;
    stream.avail_out = sizeof(output);

    zstat = inflateInit(&stream);
    if(zstat != Z_OK) {
	cli_warnmsg("cli_pdf: inflateInit failed\n");
	return CL_EMEM;
    }

    nbytes = 0;
    while(stream.avail_in) {
	zstat = inflate(&stream, Z_NO_FLUSH);	/* zlib */
	switch(zstat) {
	    case Z_OK:
		if(stream.avail_out == 0) {
		    int written;
		    if ((written=filter_writen(pdf, obj, fout, output, sizeof(output), sum))!=sizeof(output)) {
			cli_errmsg("cli_pdf: failed to write output file\n");
			inflateEnd(&stream);
			return CL_EWRITE;
		    }
		    nbytes += written;
		    stream.next_out = (Bytef *)output;
		    stream.avail_out = sizeof(output);
		}
		continue;
	    case Z_STREAM_END:
		break;
	    default:
		if(stream.msg)
		    cli_dbgmsg("cli_pdf: after writing %lu bytes, got error \"%s\" inflating PDF stream in %u %u obj\n",
			       (unsigned long)nbytes,
			       stream.msg, obj->id>>8, obj->id&0xff);
		else
		    cli_dbgmsg("cli_pdf: after writing %lu bytes, got error %d inflating PDF stream in %u %u obj\n",
			       (unsigned long)nbytes, zstat, obj->id>>8, obj->id&0xff);
		pdf->flags |= 1 << BAD_FLATE;
		inflateEnd(&stream);
		return CL_CLEAN;
	}
	break;
    }

    if(stream.avail_out != sizeof(output)) {
	if(filter_writen(pdf, obj, fout, output, sizeof(output) - stream.avail_out, sum) < 0) {
	    cli_errmsg("cli_pdf: failed to write output file\n");
	    inflateEnd(&stream);
	    return CL_EWRITE;
	}
    }

    inflateEnd(&stream);
    return CL_CLEAN;
}