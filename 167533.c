poppler_fonts_iter_is_embedded (PopplerFontsIter *iter)
{
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	return info->getEmbedded();
}