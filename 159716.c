static bool encode_pdu(struct bt_att *att, struct att_send_op *op,
					const void *pdu, uint16_t length)
{
	uint16_t pdu_len = 1;
	struct sign_info *sign = att->local_sign;
	uint32_t sign_cnt;

	if (sign && (op->opcode & ATT_OP_SIGNED_MASK))
		pdu_len += BT_ATT_SIGNATURE_LEN;

	if (length && pdu)
		pdu_len += length;

	if (pdu_len > att->mtu)
		return false;

	op->len = pdu_len;
	op->pdu = malloc(op->len);
	if (!op->pdu)
		return false;

	((uint8_t *) op->pdu)[0] = op->opcode;
	if (pdu_len > 1)
		memcpy(op->pdu + 1, pdu, length);

	if (!sign || !(op->opcode & ATT_OP_SIGNED_MASK) || !att->crypto)
		return true;

	if (!sign->counter(&sign_cnt, sign->user_data))
		goto fail;

	if ((bt_crypto_sign_att(att->crypto, sign->key, op->pdu, 1 + length,
				sign_cnt, &((uint8_t *) op->pdu)[1 + length])))
		return true;

	util_debug(att->debug_callback, att->debug_data,
					"ATT unable to generate signature");

fail:
	free(op->pdu);
	return false;
}