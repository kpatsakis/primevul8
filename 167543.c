poppler_fonts_iter_is_subset (PopplerFontsIter *iter)
{
	FontInfo *info;

	info = (FontInfo *)iter->items->get (iter->index);

	return info->getSubset();
}