static void destroy_att_send_op(void *data)
{
	struct att_send_op *op = data;

	if (op->timeout_id)
		timeout_remove(op->timeout_id);

	if (op->destroy)
		op->destroy(op->user_data);

	free(op->pdu);
	free(op);
}