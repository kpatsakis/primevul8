hexdump(const char *msg, const char *msg2, void *base, size_t len) {
	unsigned char *p;
	unsigned int cnt;

	p = base;
	cnt = 0;

	printf("*** %s [%s] (%u bytes @ %p)\n", msg, msg2, (unsigned)len, base);

	while (cnt < len) {
		if (cnt % 16 == 0)
			printf("%p: ", p);
		else if (cnt % 8 == 0)
			printf(" |");
		printf(" %02x %c", *p, (isprint(*p) ? *p : ' '));
		p++;
		cnt++;

		if (cnt % 16 == 0)
			printf("\n");
	}

	if (cnt % 16 != 0)
		printf("\n");
}