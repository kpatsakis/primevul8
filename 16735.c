gpg_ctx_get_diagnostics (struct _GpgCtx *gpg)
{
	if (!gpg->diagflushed) {
		gpg->diagflushed = TRUE;
		camel_stream_flush (gpg->diagnostics, NULL, NULL);
		if (gpg->diagbuf->len == 0)
			return NULL;

		g_byte_array_append (gpg->diagbuf, (guchar *) "", 1);
	}

	return (const gchar *) gpg->diagbuf->data;
}