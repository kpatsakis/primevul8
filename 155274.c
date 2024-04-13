mobility_opt_print(const u_char *bp, const unsigned len)
{
	unsigned i, optlen;

	for (i = 0; i < len; i += optlen) {
		if (bp[i] == IP6MOPT_PAD1)
			optlen = 1;
		else {
			if (i + 1 < len)
				optlen = bp[i + 1] + 2;
			else
				goto trunc;
		}
		if (i + optlen > len)
			goto trunc;

		switch (bp[i]) {
		case IP6MOPT_PAD1:
			printf("(pad1)");
			break;
		case IP6MOPT_PADN:
			if (len - i < IP6MOPT_MINLEN) {
				printf("(padn: trunc)");
				goto trunc;
			}
			printf("(padn)");
			break;
		case IP6MOPT_REFRESH:
			if (len - i < IP6MOPT_REFRESH_MINLEN) {
				printf("(refresh: trunc)");
				goto trunc;
			}
			/* units of 4 secs */
			printf("(refresh: %u)",
				EXTRACT_16BITS(&bp[i+2]) << 2);
			break;
		case IP6MOPT_ALTCOA:
			if (len - i < IP6MOPT_ALTCOA_MINLEN) {
				printf("(altcoa: trunc)");
				goto trunc;
			}
			printf("(alt-CoA: %s)", ip6addr_string(&bp[i+2]));
			break;
		case IP6MOPT_NONCEID:
			if (len - i < IP6MOPT_NONCEID_MINLEN) {
				printf("(ni: trunc)");
				goto trunc;
			}
			printf("(ni: ho=0x%04x co=0x%04x)",
				EXTRACT_16BITS(&bp[i+2]),
				EXTRACT_16BITS(&bp[i+4]));
			break;
		case IP6MOPT_AUTH:
			if (len - i < IP6MOPT_AUTH_MINLEN) {
				printf("(auth: trunc)");
				goto trunc;
			}
			printf("(auth)");
			break;
		default:
			if (len - i < IP6MOPT_MINLEN) {
				printf("(sopt_type %u: trunc)", bp[i]);
				goto trunc;
			}
			printf("(type-0x%02x: len=%u)", bp[i], bp[i + 1]);
			break;
		}
	}
	return;

trunc:
	printf("[trunc] ");
}