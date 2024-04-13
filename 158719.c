static int filter_writen(struct pdf_struct *pdf, struct pdf_obj *obj,
			 int fout, const char *buf, off_t len, off_t *sum)
{
    if (cli_checklimits("pdf", pdf->ctx, *sum, 0, 0))
	return len; /* pretend it was a successful write to suppress CL_EWRITE */
    *sum += len;
    return cli_writen(fout, buf, len);
}