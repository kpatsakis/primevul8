unsigned int bt_att_chan_send(struct bt_att_chan *chan, uint8_t opcode,
				const void *pdu, uint16_t len,
				bt_att_response_func_t callback,
				void *user_data,
				bt_att_destroy_func_t destroy)
{
	struct att_send_op *op;

	if (!chan || !chan->att)
		return -EINVAL;

	op = create_att_send_op(chan->att, opcode, pdu, len, callback,
						user_data, destroy);
	if (!op)
		return -EINVAL;

	if (!queue_push_tail(chan->queue, op)) {
		free(op->pdu);
		free(op);
		return 0;
	}

	wakeup_chan_writer(chan, NULL);

	return op->id;
}