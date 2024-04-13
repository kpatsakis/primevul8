static void disc_att_send_op(void *data)
{
	struct att_send_op *op = data;

	if (op->callback)
		op->callback(BT_ATT_OP_ERROR_RSP, NULL, 0, op->user_data);

	destroy_att_send_op(op);
}