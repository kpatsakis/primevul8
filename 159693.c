static enum att_op_type get_op_type(uint8_t opcode)
{
	int i;

	for (i = 0; att_opcode_type_table[i].opcode; i++) {
		if (att_opcode_type_table[i].opcode == opcode)
			return att_opcode_type_table[i].type;
	}

	if (opcode & ATT_OP_CMD_MASK)
		return ATT_OP_TYPE_CMD;

	return ATT_OP_TYPE_UNKNOWN;
}