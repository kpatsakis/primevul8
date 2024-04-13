static void handle_rsp(struct bt_att_chan *chan, uint8_t opcode, uint8_t *pdu,
								ssize_t pdu_len)
{
	struct bt_att *att = chan->att;
	struct att_send_op *op = chan->pending_req;
	uint8_t req_opcode;
	uint8_t rsp_opcode;
	uint8_t *rsp_pdu = NULL;
	uint16_t rsp_pdu_len = 0;

	/*
	 * If no request is pending, then the response is unexpected. Disconnect
	 * the bearer.
	 */
	if (!op) {
		util_debug(att->debug_callback, att->debug_data,
					"(chan %p) Received unexpected ATT "
					"response", chan);
		io_shutdown(chan->io);
		return;
	}

	/*
	 * If the received response doesn't match the pending request, or if
	 * the request is malformed, end the current request with failure.
	 */
	if (opcode == BT_ATT_OP_ERROR_RSP) {
		/* Return if error response cause a retry */
		if (handle_error_rsp(chan, pdu, pdu_len, &req_opcode)) {
			wakeup_chan_writer(chan, NULL);
			return;
		}
	} else if (!(req_opcode = get_req_opcode(opcode)))
		goto fail;

	if (req_opcode != op->opcode)
		goto fail;

	rsp_opcode = opcode;

	if (pdu_len > 0) {
		rsp_pdu = pdu;
		rsp_pdu_len = pdu_len;
	}

	goto done;

fail:
	util_debug(att->debug_callback, att->debug_data,
			"(chan %p) Failed to handle response PDU; opcode: "
			"0x%02x", chan, opcode);

	rsp_opcode = BT_ATT_OP_ERROR_RSP;

done:
	if (op->callback)
		op->callback(rsp_opcode, rsp_pdu, rsp_pdu_len, op->user_data);

	destroy_att_send_op(op);
	chan->pending_req = NULL;

	wakeup_chan_writer(chan, NULL);
}