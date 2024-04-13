static bool bt_att_disc_cancel(struct bt_att *att, unsigned int id)
{
	struct att_send_op *op;

	op = queue_find(att->req_queue, match_op_id, UINT_TO_PTR(id));
	if (op)
		goto done;

	op = queue_find(att->ind_queue, match_op_id, UINT_TO_PTR(id));
	if (op)
		goto done;

	op = queue_find(att->write_queue, match_op_id, UINT_TO_PTR(id));

done:
	if (!op)
		return false;

	/* Just cancel since disconnect_cb will be cleaning up */
	cancel_att_send_op(op);

	return true;
}