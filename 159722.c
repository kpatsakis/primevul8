static bool match_op_id(const void *a, const void *b)
{
	const struct att_send_op *op = a;
	unsigned int id = PTR_TO_UINT(b);

	return op->id == id;
}