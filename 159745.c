static bool handle_error_rsp(struct bt_att_chan *chan, uint8_t *pdu,
					ssize_t pdu_len, uint8_t *opcode)
{
	struct bt_att *att = chan->att;
	const struct bt_att_pdu_error_rsp *rsp;
	struct att_send_op *op = chan->pending_req;

	if (pdu_len != sizeof(*rsp)) {
		*opcode = 0;
		return false;
	}

	rsp = (void *) pdu;

	*opcode = rsp->opcode;

	/* Attempt to change security */
	if (!change_security(chan, rsp->ecode))
		return false;

	/* Remove timeout_id if outstanding */
	if (op->timeout_id) {
		timeout_remove(op->timeout_id);
		op->timeout_id = 0;
	}

	util_debug(att->debug_callback, att->debug_data,
						"(chan %p) Retrying operation "
						"%p", chan, op);

	chan->pending_req = NULL;

	/* Push operation back to request queue */
	return queue_push_head(att->req_queue, op);
}