static int sprint_nip(char *dest, const char *pre, const uint8_t *ip)
{
	return sprintf(dest, "%s%u.%u.%u.%u", pre, ip[0], ip[1], ip[2], ip[3]);
}