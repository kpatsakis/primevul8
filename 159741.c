int bt_att_chan_send_error_rsp(struct bt_att_chan *chan, uint8_t opcode,
						uint16_t handle, int error)
{
	struct bt_att_pdu_error_rsp pdu;
	uint8_t ecode;

	if (!chan || !chan->att || !opcode)
		return -EINVAL;

	ecode = att_ecode_from_error(error);

	memset(&pdu, 0, sizeof(pdu));

	pdu.opcode = opcode;
	put_le16(handle, &pdu.handle);
	pdu.ecode = ecode;

	return bt_att_chan_send_rsp(chan, BT_ATT_OP_ERROR_RSP, &pdu,
							sizeof(pdu));
}