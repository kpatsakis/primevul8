ascii85decode(const char *buf, off_t len, unsigned char *output)
{
	const char *ptr;
	uint32_t sum = 0;
	int quintet = 0;
	int ret = 0;

	if(cli_memstr(buf, len, "~>", 2) == NULL)
		cli_dbgmsg("cli_pdf: ascii85decode: no EOF marker found\n");

	ptr = buf;

	cli_dbgmsg("cli_pdf: ascii85decode %lu bytes\n", (unsigned long)len);

	while(len > 0) {
		int byte = (len--) ? (int)*ptr++ : EOF;

		if((byte == '~') && (*ptr == '>'))
			byte = EOF;

		if(byte >= '!' && byte <= 'u') {
			sum = (sum * 85) + ((uint32_t)byte - '!');
			if(++quintet == 5) {
				*output++ = (unsigned char)(sum >> 24);
				*output++ = (unsigned char)((sum >> 16) & 0xFF);
				*output++ = (unsigned char)((sum >> 8) & 0xFF);
				*output++ = (unsigned char)(sum & 0xFF);
				ret += 4;
				quintet = 0;
				sum = 0;
			}
		} else if(byte == 'z') {
			if(quintet) {
				cli_dbgmsg("ascii85decode: unexpected 'z'\n");
				return -1;
			}
			*output++ = '\0';
			*output++ = '\0';
			*output++ = '\0';
			*output++ = '\0';
			ret += 4;
		} else if(byte == EOF) {
			cli_dbgmsg("ascii85decode: quintet %d\n", quintet);
			if(quintet) {
				int i;

				if(quintet == 1) {
					cli_dbgmsg("ascii85Decode: only 1 byte in last quintet\n");
					return -1;
				}
				for(i = quintet; i < 5; i++)
					sum *= 85;

				if(quintet > 1)
					sum += (0xFFFFFF >> ((quintet - 2) * 8));
				ret += quintet;
				for(i = 0; i < quintet - 1; i++)
					*output++ = (unsigned char)((sum >> (24 - 8 * i)) & 0xFF);
			}
			break;
		} else if(!isspace(byte)) {
			cli_dbgmsg("ascii85Decode: invalid character 0x%x, len %lu\n",
				byte & 0xFF, (unsigned long)len);
			return -1;
		}
	}
	return ret;
}