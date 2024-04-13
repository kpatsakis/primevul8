poppler_fonts_iter_new (GooList *items)
{
	PopplerFontsIter *iter;

	iter = g_slice_new (PopplerFontsIter);
	iter->items = items;
	iter->index = 0;

	return iter;
}