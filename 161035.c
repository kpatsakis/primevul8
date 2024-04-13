ipmi_1_5_authtypes(uint8_t n)
{
	uint32_t i;
	static char supportedTypes[128];

	memset(supportedTypes, 0, sizeof(supportedTypes));
	for (i = 0; ipmi_authtype_vals[i].val != 0; i++) {
		if (n & ipmi_authtype_vals[i].val) {
			strcat(supportedTypes, ipmi_authtype_vals[i].str);
			strcat(supportedTypes, " ");
		}
	}

	return supportedTypes;
}