poppler_index_iter_copy (PopplerIndexIter *iter)
{
	PopplerIndexIter *new_iter;

	g_return_val_if_fail (iter != nullptr, NULL);

	new_iter = g_slice_dup (PopplerIndexIter, iter);
	new_iter->document = (PopplerDocument *) g_object_ref (new_iter->document);

	return new_iter;
}