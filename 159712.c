bool bt_att_chan_cancel(struct bt_att_chan *chan, unsigned int id)
{
	struct att_send_op *op;

	if (chan->pending_req && chan->pending_req->id == id) {
		/* Don't cancel the pending request; remove it's handlers */
		cancel_att_send_op(chan->pending_req);
		return true;
	}

	if (chan->pending_ind && chan->pending_ind->id == id) {
		/* Don't cancel the pending indication; remove it's handlers. */
		cancel_att_send_op(chan->pending_ind);
		return true;
	}

	op = queue_remove_if(chan->queue, match_op_id, UINT_TO_PTR(id));
	if (!op)
		return false;

	destroy_att_send_op(op);

	wakeup_chan_writer(chan, NULL);

	return true;
}