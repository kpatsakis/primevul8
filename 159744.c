static void respond_not_supported(struct bt_att *att, uint8_t opcode)
{
	struct bt_att_pdu_error_rsp pdu;

	pdu.opcode = opcode;
	pdu.handle = 0x0000;
	pdu.ecode = BT_ATT_ERROR_REQUEST_NOT_SUPPORTED;

	bt_att_send(att, BT_ATT_OP_ERROR_RSP, &pdu, sizeof(pdu), NULL, NULL,
									NULL);
}