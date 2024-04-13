poppler_font_info_new (PopplerDocument *document)
{
	PopplerFontInfo *font_info;

	g_return_val_if_fail (POPPLER_IS_DOCUMENT (document), NULL);

	font_info = (PopplerFontInfo *) g_object_new (POPPLER_TYPE_FONT_INFO,
						      nullptr);
	font_info->document = (PopplerDocument *) g_object_ref (document);
	font_info->scanner = new FontInfoScanner(document->doc);

	return font_info;
}