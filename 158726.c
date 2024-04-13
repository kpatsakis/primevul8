flatedecode(unsigned char *buf, off_t len, int fout, cli_ctx *ctx)
{
	int zstat, ret;
	off_t nbytes;
	z_stream stream;
	unsigned char output[BUFSIZ];
#ifdef	SAVE_TMP
	char tmpfilename[16];
	int tmpfd;
#endif

	cli_dbgmsg("cli_pdf: flatedecode %lu bytes\n", (unsigned long)len);

	if(len == 0) {
		cli_dbgmsg("cli_pdf: flatedecode len == 0\n");
		return CL_CLEAN;
	}

#ifdef	SAVE_TMP
	/*
	 * Copy the embedded area for debugging, so that if it falls over
	 * we have a copy of the offending data. This is debugging code
	 * that you shouldn't of course install in a live environment. I am
	 * not interested in hearing about security issues with this section
	 * of the parser.
	 */
	strcpy(tmpfilename, "/tmp/pdfXXXXXX");
	tmpfd = mkstemp(tmpfilename);
	if(tmpfd < 0) {
		perror(tmpfilename);
		cli_errmsg("cli_pdf: Can't make debugging file\n");
	} else {
		FILE *tmpfp = fdopen(tmpfd, "w");

		if(tmpfp) {
			fwrite(buf, sizeof(char), len, tmpfp);
			fclose(tmpfp);
			cli_dbgmsg("cli_pdf: flatedecode: debugging file is %s\n",
				tmpfilename);
		} else
			cli_errmsg("cli_pdf: can't fdopen debugging file\n");
	}
#endif
	stream.zalloc = (alloc_func)Z_NULL;
	stream.zfree = (free_func)Z_NULL;
	stream.opaque = (void *)NULL;
	stream.next_in = (Bytef *)buf;
	stream.avail_in = len;
	stream.next_out = output;
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
					if ((written=cli_writen(fout, output, sizeof(output)))!=sizeof(output)) {
						cli_errmsg("cli_pdf: failed to write output file\n");
						inflateEnd(&stream);
						return CL_EWRITE;
					}
					nbytes += written;

					if((ret=cli_checklimits("cli_pdf", ctx, nbytes, 0, 0))!=CL_CLEAN) {
						inflateEnd(&stream);
						return ret;
					}
					stream.next_out = output;
					stream.avail_out = sizeof(output);
				}
				continue;
			case Z_STREAM_END:
				break;
			default:
				if(stream.msg)
					cli_dbgmsg("cli_pdf: after writing %lu bytes, got error \"%s\" inflating PDF attachment\n",
						(unsigned long)nbytes,
						stream.msg);
				else
					cli_dbgmsg("cli_pdf: after writing %lu bytes, got error %d inflating PDF attachment\n",
						(unsigned long)nbytes, zstat);
				inflateEnd(&stream);
				return CL_CLEAN;
		}
		break;
	}

	if(stream.avail_out != sizeof(output)) {
		if(cli_writen(fout, output, sizeof(output) - stream.avail_out) < 0) {
			cli_errmsg("cli_pdf: failed to write output file\n");
			inflateEnd(&stream);
			return CL_EWRITE;
		}
	}
			
#ifdef	SAVE_TMP
	if (cli_unlink(tmpfilename)) {
		inflateEnd(&stream);
		return CL_EUNLINK;
	}
#endif
	inflateEnd(&stream);
	return CL_CLEAN;
}