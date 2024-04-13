try_flatedecode(unsigned char *buf, off_t real_len, off_t calculated_len, int fout, cli_ctx *ctx)
{
	int ret = cli_checklimits("cli_pdf", ctx, real_len, 0, 0);

	if (ret==CL_CLEAN && flatedecode(buf, real_len, fout, ctx) == CL_SUCCESS)
		return CL_CLEAN;

	if(real_len == calculated_len) {
		/*
		 * Nothing more we can do to inflate
		 */
		cli_dbgmsg("cli_pdf: Bad compression in flate stream\n");
		return CL_CLEAN;
	}

	if(cli_checklimits("cli_pdf", ctx, calculated_len, 0, 0)!=CL_CLEAN)
		return CL_CLEAN;

	ret = flatedecode(buf, calculated_len, fout, ctx);
	if(ret == CL_CLEAN)
		return CL_CLEAN;

	/* i.e. the PDF file is broken :-( */
	cli_dbgmsg("cli_pdf: Bad compressed block length in flate stream\n");

	return ret;
}