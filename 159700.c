static void bt_att_chan_free(void *data)
{
	struct bt_att_chan *chan = data;

	if (chan->pending_req)
		destroy_att_send_op(chan->pending_req);

	if (chan->pending_ind)
		destroy_att_send_op(chan->pending_ind);

	queue_destroy(chan->queue, destroy_att_send_op);

	io_destroy(chan->io);

	free(chan->buf);
	free(chan);
}