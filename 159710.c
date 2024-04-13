static ssize_t bt_att_chan_write(struct bt_att_chan *chan, uint8_t opcode,
					const void *pdu, uint16_t len)
{
	struct bt_att *att = chan->att;
	ssize_t ret;
	struct iovec iov;

	iov.iov_base = (void *) pdu;
	iov.iov_len = len;

	util_debug(att->debug_callback, att->debug_data,
					"(chan %p) ATT op 0x%02x",
					chan, opcode);

	ret = io_send(chan->io, &iov, 1);
	if (ret < 0) {
		util_debug(att->debug_callback, att->debug_data,
					"(chan %p) write failed: %s",
					chan, strerror(-ret));

		return ret;
	}

	util_hexdump('<', pdu, ret, att->debug_callback, att->debug_data);

	return ret;
}