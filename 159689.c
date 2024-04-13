static bool handle_signed(struct bt_att *att, uint8_t *pdu, ssize_t pdu_len)
{
	uint8_t *signature;
	uint32_t sign_cnt;
	struct sign_info *sign;
	uint8_t opcode = pdu[0];

	/* Check if there is enough data for a signature */
	if (pdu_len < 3 + BT_ATT_SIGNATURE_LEN)
		goto fail;

	sign = att->remote_sign;
	if (!sign)
		goto fail;

	signature = pdu + (pdu_len - BT_ATT_SIGNATURE_LEN);
	sign_cnt = get_le32(signature);

	/* Validate counter */
	if (!sign->counter(&sign_cnt, sign->user_data))
		goto fail;

	/* Verify received signature */
	if (!bt_crypto_verify_att_sign(att->crypto, sign->key, pdu, pdu_len))
		goto fail;

	return true;

fail:
	util_debug(att->debug_callback, att->debug_data,
			"ATT failed to verify signature: 0x%02x", opcode);

	return false;
}