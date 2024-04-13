static bool opcode_match(uint8_t opcode, uint8_t test_opcode)
{
	enum att_op_type op_type = get_op_type(test_opcode);

	if (opcode == BT_ATT_ALL_REQUESTS && (op_type == ATT_OP_TYPE_REQ ||
						op_type == ATT_OP_TYPE_CMD))
		return true;

	return opcode == test_opcode;
}