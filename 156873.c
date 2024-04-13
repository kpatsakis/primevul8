in_csum(const uint16_t *addr, size_t len, uint32_t csum, uint32_t *acc)
{
	register size_t nleft = len;
	const uint16_t *w = addr;
	register uint16_t answer;
	register uint32_t sum = csum;

	/*
	 *  Our algorithm is simple, using a 32 bit accumulator (sum),
	 *  we add sequential 16 bit words to it, and at the end, fold
	 *  back all the carry bits from the top 16 bits into the lower
	 *  16 bits.
	 */
	while (nleft > 1) {
		sum += *w++;
		nleft -= 2;
	}

	/* mop up an odd byte, if necessary */
	if (nleft == 1)
		sum += htons(*(u_char *) w << 8);

	if (acc)
		*acc = sum;

	/*
	 * add back carry outs from top 16 bits to low 16 bits
	 */
	sum = (sum >> 16) + (sum & 0xffff);	/* add hi 16 to low 16 */
	sum += (sum >> 16);			/* add carry */
	answer = (~sum & 0xffff);		/* truncate to 16 bits */
	return (answer);
}