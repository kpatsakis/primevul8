static bool timeout_cb(void *user_data)
{
	struct timeout_data *timeout = user_data;
	struct bt_att_chan *chan = timeout->chan;
	struct bt_att *att = chan->att;
	struct att_send_op *op = NULL;

	if (chan->pending_req && chan->pending_req->id == timeout->id) {
		op = chan->pending_req;
		chan->pending_req = NULL;
	} else if (chan->pending_ind && chan->pending_ind->id == timeout->id) {
		op = chan->pending_ind;
		chan->pending_ind = NULL;
	}

	if (!op)
		return false;

	util_debug(att->debug_callback, att->debug_data,
				"(chan %p) Operation timed out: 0x%02x",
				chan, op->opcode);

	if (att->timeout_callback)
		att->timeout_callback(op->id, op->opcode, att->timeout_data);

	op->timeout_id = 0;
	disc_att_send_op(op);

	/*
	 * Directly terminate the connection as required by the ATT protocol.
	 * This should trigger an io disconnect event which will clean up the
	 * io and notify the upper layer.
	 */
	io_shutdown(chan->io);

	return false;
}