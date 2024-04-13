static char *malloc_option_value_string(uint8_t *option, GDHCPOptionType type)
{
	unsigned upper_length;
	int len, optlen;
	char *dest, *ret;

	len = option[OPT_LEN - OPT_DATA];
	type &= OPTION_TYPE_MASK;
	optlen = dhcp_option_lengths[type];
	if (optlen == 0)
		return NULL;
	upper_length = len_of_option_as_string[type] *
			((unsigned)len / (unsigned)optlen);
	dest = ret = g_malloc(upper_length + 1);
	if (!ret)
		return NULL;

	while (len >= optlen) {
		switch (type) {
		case OPTION_IP:
			dest += sprint_nip(dest, "", option);
			break;
		case OPTION_U16: {
			uint16_t val_u16 = get_be16(option);
			dest += sprintf(dest, "%u", val_u16);
			break;
		}
		case OPTION_U32: {
			uint32_t val_u32 = get_be32(option);
			dest += sprintf(dest, "%u", val_u32);
			break;
		}
		case OPTION_STRING:
			memcpy(dest, option, len);
			dest[len] = '\0';
			return ret;
		default:
			break;
		}
		option += optlen;
		len -= optlen;
		if (len <= 0)
			break;
		*dest++ = ' ';
		*dest = '\0';
	}

	return ret;
}