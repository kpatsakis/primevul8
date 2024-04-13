poppler_index_iter_next (PopplerIndexIter *iter)
{
	g_return_val_if_fail (iter != nullptr, FALSE);

	iter->index++;
	if (iter->index >= iter->items->getLength())
		return FALSE;

	return TRUE;
}