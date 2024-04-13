static void pdfobj_flag(struct pdf_struct *pdf, struct pdf_obj *obj, enum pdf_flag flag)
{
    const char *s;
    pdf->flags |= 1 << flag;
    if (!cli_debug_flag)
	return;
    switch (flag) {
	case UNTERMINATED_OBJ_DICT:
	    s = "dictionary not terminated";
	    break;
	case ESCAPED_COMMON_PDFNAME:
	    /* like /JavaScript */
	    s = "escaped common pdfname";
	    break;
	case BAD_STREAM_FILTERS:
	    s = "duplicate stream filters";
	    break;
	default:
	case BAD_PDF_VERSION:
	case BAD_PDF_HEADERPOS:
	case BAD_PDF_TRAILER:
	case BAD_PDF_TOOMANYOBJS:
	    return;
    }
    cli_dbgmsg("cli_pdf: %s in object %u %u\n", s, obj->id>>8, obj->id&0xff);
}