poppler_fonts_iter_next (PopplerFontsIter *iter)
{
	g_return_val_if_fail (iter != nullptr, FALSE);

	iter->index++;
	if (iter->index >= iter->items->getLength())
		return FALSE;

	return TRUE;
}