swrite (CamelMimePart *sigpart,
        GCancellable *cancellable,
        GError **error)
{
	CamelStream *ostream;
	CamelDataWrapper *wrapper;
	gchar *template;
	gint fd, ret;

	template = g_build_filename (g_get_tmp_dir (), "evolution-pgp.XXXXXX", NULL);
	if ((fd = g_mkstemp (template)) == -1) {
		g_free (template);
		return NULL;
	}

	ostream = camel_stream_fs_new_with_fd (fd);
	wrapper = camel_medium_get_content (CAMEL_MEDIUM (sigpart));
	if (!wrapper)
		wrapper = CAMEL_DATA_WRAPPER (sigpart);

	ret = camel_data_wrapper_decode_to_stream_sync (
		wrapper, ostream, cancellable, error);
	if (ret != -1) {
		ret = camel_stream_flush (ostream, cancellable, error);
		if (ret != -1)
			ret = camel_stream_close (ostream, cancellable, error);
	}

	g_object_unref (ostream);

	if (ret == -1) {
		g_unlink (template);
		g_free (template);
		return NULL;
	}

	return template;
}