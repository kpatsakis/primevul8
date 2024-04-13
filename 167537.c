poppler_index_iter_is_open (PopplerIndexIter *iter)
{
	OutlineItem *item;

	item = (OutlineItem *)iter->items->get (iter->index);

	return item->isOpen();
}