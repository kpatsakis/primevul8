poppler_document_find_dest (PopplerDocument *document,
			    const gchar     *link_name)
{
	PopplerDest *dest = nullptr;
	LinkDest *link_dest = nullptr;
	GooString *g_link_name;

	g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);
	g_return_val_if_fail (link_name != nullptr, NULL);

	g_link_name = new GooString (link_name);

	if (g_link_name) {
		link_dest = document->doc->findDest (g_link_name);
		delete g_link_name;
	}

	if (link_dest) {
		dest = _poppler_dest_new_goto (document, link_dest);
		delete link_dest;
	}

	return dest;
}