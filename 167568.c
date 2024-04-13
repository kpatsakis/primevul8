poppler_fonts_iter_free (PopplerFontsIter *iter)
{
	if (G_UNLIKELY (iter == nullptr))
		return;

	deleteGooList<FontInfo> (iter->items);

	g_slice_free (PopplerFontsIter, iter);
}