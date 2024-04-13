poppler_fonts_iter_get_font_type (PopplerFontsIter *iter)
{
	FontInfo *info;

	g_return_val_if_fail (iter != nullptr, POPPLER_FONT_TYPE_UNKNOWN);

	info = (FontInfo *)iter->items->get (iter->index);

	return (PopplerFontType)info->getType ();
}