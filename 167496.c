poppler_ps_file_new (PopplerDocument *document, const char *filename,
		     int first_page, int n_pages)
{
	PopplerPSFile *ps_file;

	g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);
	g_return_val_if_fail (filename != nullptr, NULL);
	g_return_val_if_fail (n_pages > 0, NULL);

        ps_file = (PopplerPSFile *) g_object_new (POPPLER_TYPE_PS_FILE, nullptr);
	ps_file->document = (PopplerDocument *) g_object_ref (document);
        ps_file->filename = g_strdup (filename);
        ps_file->first_page = first_page + 1;
        ps_file->last_page = first_page + 1 + n_pages - 1;

	return ps_file;
}