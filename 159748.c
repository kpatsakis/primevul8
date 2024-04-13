bool bt_att_cancel_all(struct bt_att *att)
{
	const struct queue_entry *entry;

	if (!att)
		return false;

	queue_remove_all(att->req_queue, NULL, NULL, destroy_att_send_op);
	queue_remove_all(att->ind_queue, NULL, NULL, destroy_att_send_op);
	queue_remove_all(att->write_queue, NULL, NULL, destroy_att_send_op);

	for (entry = queue_get_entries(att->chans); entry;
						entry = entry->next) {
		struct bt_att_chan *chan = entry->data;

		if (chan->pending_req)
			/* Don't cancel the pending request; remove it's
			 * handlers
			 */
			cancel_att_send_op(chan->pending_req);

		if (chan->pending_ind)
			/* Don't cancel the pending request; remove it's
			 * handlers
			 */
			cancel_att_send_op(chan->pending_ind);
	}

	return true;
}