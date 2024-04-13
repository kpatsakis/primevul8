poppler_font_info_free (PopplerFontInfo *font_info)
{
	g_return_if_fail (font_info != nullptr);

	g_object_unref (font_info);
}