fatal_error_handler (j_common_ptr cinfo)
{
	struct error_handler_data *errmgr;

	errmgr = (struct error_handler_data *) cinfo->err;
	if ((errmgr->error != NULL) && (*errmgr->error == NULL)) {
		char buffer[JMSG_LENGTH_MAX];

		/* Create the message */
		(* cinfo->err->format_message) (cinfo, buffer);

		g_set_error (errmgr->error,
			     GDK_PIXBUF_ERROR,
			     GDK_PIXBUF_ERROR_CORRUPT_IMAGE,
			     _("Error interpreting JPEG image file: %s"),
			     buffer);
		siglongjmp (errmgr->setjmp_buffer, 1);
	}
}