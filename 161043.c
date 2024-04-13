iana_string(uint32_t iana)
{
	static char s[10];

	if (iana) {
		sprintf(s, "%06x", iana);
		return s;
	} else {
		return "N/A";
	}
}