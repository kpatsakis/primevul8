static struct att_send_op *pick_next_send_op(struct bt_att_chan *chan)
{
	struct bt_att *att = chan->att;
	struct att_send_op *op;

	/* Check if there is anything queued on the channel */
	op = queue_pop_head(chan->queue);
	if (op)
		return op;

	/* See if any operations are already in the write queue */
	op = queue_peek_head(att->write_queue);
	if (op && op->len <= chan->mtu)
		return queue_pop_head(att->write_queue);

	/* If there is no pending request, pick an operation from the
	 * request queue.
	 */
	if (!chan->pending_req) {
		op = queue_peek_head(att->req_queue);
		if (op && op->len <= chan->mtu)
			return queue_pop_head(att->req_queue);
	}

	/* There is either a request pending or no requests queued. If there is
	 * no pending indication, pick an operation from the indication queue.
	 */
	if (!chan->pending_ind) {
		op = queue_peek_head(att->ind_queue);
		if (op && op->len <= chan->mtu)
			return queue_pop_head(att->ind_queue);
	}

	return NULL;
}