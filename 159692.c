bool bt_att_set_close_on_unref(struct bt_att *att, bool do_close)
{
	const struct queue_entry *entry;

	if (!att)
		return false;

	att->close_on_unref = do_close;

	for (entry = queue_get_entries(att->chans); entry;
						entry = entry->next) {
		struct bt_att_chan *chan = entry->data;

		if (!io_set_close_on_destroy(chan->io, do_close))
			return false;
	}

	return true;
}