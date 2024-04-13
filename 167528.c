poppler_index_iter_new (PopplerDocument *document)
{
	PopplerIndexIter *iter;
	Outline *outline;
	const GooList *items;

	outline = document->doc->getOutline();
	if (outline == nullptr)
		return nullptr;

	items = outline->getItems();
	if (items == nullptr)
		return nullptr;

	iter = g_slice_new (PopplerIndexIter);
	iter->document = (PopplerDocument *) g_object_ref (document);
	iter->items = items;
	iter->index = 0;

	return iter;
}