static unsigned int handle_diacr(struct vc_data *vc, unsigned int ch)
{
	unsigned int d = diacr;
	unsigned int i;

	diacr = 0;

	if ((d & ~0xff) == BRL_UC_ROW) {
		if ((ch & ~0xff) == BRL_UC_ROW)
			return d | ch;
	} else {
		for (i = 0; i < accent_table_size; i++)
			if (accent_table[i].diacr == d && accent_table[i].base == ch)
				return accent_table[i].result;
	}

	if (ch == ' ' || ch == (BRL_UC_ROW|0) || ch == d)
		return d;

	if (kbd->kbdmode == VC_UNICODE)
		to_utf8(vc, d);
	else {
		int c = conv_uni_to_8bit(d);
		if (c != -1)
			put_queue(vc, c);
	}

	return ch;
}