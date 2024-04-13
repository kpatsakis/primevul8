static bool is_valid_ie_attr(const struct nlattr *attr)
{
	const u8 *pos;
	int len;

	if (!attr)
		return true;

	pos = nla_data(attr);
	len = nla_len(attr);

	while (len) {
		u8 elemlen;

		if (len < 2)
			return false;
		len -= 2;

		elemlen = pos[1];
		if (elemlen > len)
			return false;

		len -= elemlen;
		pos += 2 + elemlen;
	}

	return true;
}