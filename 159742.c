unsigned int bt_att_send(struct bt_att *att, uint8_t opcode,
				const void *pdu, uint16_t length,
				bt_att_response_func_t callback, void *user_data,
				bt_att_destroy_func_t destroy)
{
	struct att_send_op *op;
	bool result;

	if (!att || queue_isempty(att->chans))
		return 0;

	op = create_att_send_op(att, opcode, pdu, length, callback, user_data,
								destroy);
	if (!op)
		return 0;

	if (att->next_send_id < 1)
		att->next_send_id = 1;

	op->id = att->next_send_id++;

	/* Add the op to the correct queue based on its type */
	switch (op->type) {
	case ATT_OP_TYPE_REQ:
		result = queue_push_tail(att->req_queue, op);
		break;
	case ATT_OP_TYPE_IND:
		result = queue_push_tail(att->ind_queue, op);
		break;
	case ATT_OP_TYPE_CMD:
	case ATT_OP_TYPE_NFY:
	case ATT_OP_TYPE_UNKNOWN:
	case ATT_OP_TYPE_RSP:
	case ATT_OP_TYPE_CONF:
	default:
		result = queue_push_tail(att->write_queue, op);
		break;
	}

	if (!result) {
		free(op->pdu);
		free(op);
		return 0;
	}

	wakeup_writer(att);

	return op->id;
}