poppler_index_iter_free (PopplerIndexIter *iter)
{
	if (G_UNLIKELY (iter == nullptr))
		return;

	g_object_unref (iter->document);
	g_slice_free (PopplerIndexIter, iter);
}