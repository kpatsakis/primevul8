static struct att_send_op *create_att_send_op(struct bt_att *att,
						uint8_t opcode,
						const void *pdu,
						uint16_t length,
						bt_att_response_func_t callback,
						void *user_data,
						bt_att_destroy_func_t destroy)
{
	struct att_send_op *op;
	enum att_op_type type;

	if (length && !pdu)
		return NULL;

	type = get_op_type(opcode);
	if (type == ATT_OP_TYPE_UNKNOWN)
		return NULL;

	/* If the opcode corresponds to an operation type that does not elicit a
	 * response from the remote end, then no callback should have been
	 * provided, since it will never be called.
	 */
	if (callback && type != ATT_OP_TYPE_REQ && type != ATT_OP_TYPE_IND)
		return NULL;

	/* Similarly, if the operation does elicit a response then a callback
	 * must be provided.
	 */
	if (!callback && (type == ATT_OP_TYPE_REQ || type == ATT_OP_TYPE_IND))
		return NULL;

	op = new0(struct att_send_op, 1);
	op->type = type;
	op->opcode = opcode;
	op->callback = callback;
	op->destroy = destroy;
	op->user_data = user_data;

	if (!encode_pdu(att, op, pdu, length)) {
		free(op);
		return NULL;
	}

	return op;
}