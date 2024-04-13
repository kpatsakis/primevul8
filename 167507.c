poppler_fonts_iter_copy (PopplerFontsIter *iter)
{
	PopplerFontsIter *new_iter;

	g_return_val_if_fail (iter != nullptr, NULL);

	new_iter = g_slice_dup (PopplerFontsIter, iter);

	new_iter->items = new GooList ();
	for (int i = 0; i < iter->items->getLength(); i++) {
		FontInfo *info = (FontInfo *)iter->items->get(i);
		new_iter->items->push_back (new FontInfo (*info));
	}

	return new_iter;
}