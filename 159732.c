static bool can_read_data(struct io *io, void *user_data)
{
	struct bt_att_chan *chan = user_data;
	struct bt_att *att = chan->att;
	uint8_t opcode;
	uint8_t *pdu;
	ssize_t bytes_read;

	bytes_read = read(chan->fd, chan->buf, chan->mtu);
	if (bytes_read < 0)
		return false;

	util_debug(att->debug_callback, att->debug_data,
				"(chan %p) ATT received: %zd",
				chan, bytes_read);

	util_hexdump('>', chan->buf, bytes_read,
				att->debug_callback, att->debug_data);

	if (bytes_read < ATT_MIN_PDU_LEN)
		return true;

	pdu = chan->buf;
	opcode = pdu[0];

	bt_att_ref(att);

	/* Act on the received PDU based on the opcode type */
	switch (get_op_type(opcode)) {
	case ATT_OP_TYPE_RSP:
		util_debug(att->debug_callback, att->debug_data,
				"(chan %p) ATT response received: 0x%02x",
				chan, opcode);
		handle_rsp(chan, opcode, pdu + 1, bytes_read - 1);
		break;
	case ATT_OP_TYPE_CONF:
		util_debug(att->debug_callback, att->debug_data,
				"(chan %p) ATT confirmation received: 0x%02x",
				chan, opcode);
		handle_conf(chan, pdu + 1, bytes_read - 1);
		break;
	case ATT_OP_TYPE_REQ:
		/*
		 * If a request is currently pending, then the sequential
		 * protocol was violated. Disconnect the bearer, which will
		 * promptly notify the upper layer via disconnect handlers.
		 */
		if (chan->in_req) {
			util_debug(att->debug_callback, att->debug_data,
					"(chan %p) Received request while "
					"another is pending: 0x%02x",
					chan, opcode);
			io_shutdown(chan->io);
			bt_att_unref(chan->att);

			return false;
		}

		chan->in_req = true;
		/* fall through */
	case ATT_OP_TYPE_CMD:
	case ATT_OP_TYPE_NFY:
	case ATT_OP_TYPE_UNKNOWN:
	case ATT_OP_TYPE_IND:
		/* fall through */
	default:
		/* For all other opcodes notify the upper layer of the PDU and
		 * let them act on it.
		 */
		util_debug(att->debug_callback, att->debug_data,
					"(chan %p) ATT PDU received: 0x%02x",
					chan, opcode);
		handle_notify(chan, pdu, bytes_read);
		break;
	}

	bt_att_unref(att);

	return true;
}