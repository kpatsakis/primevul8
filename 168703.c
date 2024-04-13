static const char* protocol_security_string(UINT32 security)
{
	static const char* PROTOCOL_SECURITY_STRINGS[] = { "RDP", "TLS", "NLA", "UNK", "UNK",
		                                               "UNK", "UNK", "UNK", "EXT", "UNK" };
	if (security >= ARRAYSIZE(PROTOCOL_SECURITY_STRINGS))
		return PROTOCOL_SECURITY_STRINGS[ARRAYSIZE(PROTOCOL_SECURITY_STRINGS) - 1];
	return PROTOCOL_SECURITY_STRINGS[security];
}