static int asciihexdecode(const char *buf, off_t len, char *output)
{
    unsigned i,j;
    for (i=0,j=0;i<len;i++) {
	if (buf[i] == ' ')
	    continue;
	if (buf[i] == '>')
	    break;
	cli_hex2str_to(buf+i, output+j++, 2);
	i++;
    }
    return j;
}