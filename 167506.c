poppler_index_iter_get_action (PopplerIndexIter  *iter)
{
	OutlineItem *item;
	const LinkAction *link_action;
	PopplerAction *action;
	gchar *title;

	g_return_val_if_fail (iter != nullptr, NULL);

	item = (OutlineItem *)iter->items->get (iter->index);
	link_action = item->getAction ();

	title = unicode_to_char (item->getTitle(),
				 item->getTitleLength ());

	action = _poppler_action_new (iter->document, link_action, title);
	g_free (title);

	return action;
}