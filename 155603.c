compute_cookie(ns_client_t *client, uint32_t when, uint32_t nonce,
	       const unsigned char *secret, isc_buffer_t *buf) {
	unsigned char digest[ISC_MAX_MD_SIZE] ISC_NONSTRING = { 0 };
	STATIC_ASSERT(ISC_MAX_MD_SIZE >= ISC_SIPHASH24_TAG_LENGTH, "You need "
								   "to "
								   "increase "
								   "the digest "
								   "buffer.");
	STATIC_ASSERT(ISC_MAX_MD_SIZE >= ISC_AES_BLOCK_LENGTH, "You need to "
							       "increase the "
							       "digest "
							       "buffer.");

	switch (client->sctx->cookiealg) {
	case ns_cookiealg_siphash24: {
		unsigned char input[16 + 16] ISC_NONSTRING = { 0 };
		size_t inputlen = 0;
		isc_netaddr_t netaddr;
		unsigned char *cp;

		cp = isc_buffer_used(buf);
		isc_buffer_putmem(buf, client->cookie, 8);
		isc_buffer_putuint8(buf, NS_COOKIE_VERSION_1);
		isc_buffer_putuint24(buf, 0); /* Reserved */
		isc_buffer_putuint32(buf, when);

		memmove(input, cp, 16);

		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		switch (netaddr.family) {
		case AF_INET:
			cp = (unsigned char *)&netaddr.type.in;
			memmove(input + 16, cp, 4);
			inputlen = 20;
			break;
		case AF_INET6:
			cp = (unsigned char *)&netaddr.type.in6;
			memmove(input + 16, cp, 16);
			inputlen = 32;
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}

		isc_siphash24(secret, input, inputlen, digest);
		isc_buffer_putmem(buf, digest, 8);
		break;
	}
	case ns_cookiealg_aes: {
		unsigned char input[4 + 4 + 16] ISC_NONSTRING = { 0 };
		isc_netaddr_t netaddr;
		unsigned char *cp;
		unsigned int i;

		cp = isc_buffer_used(buf);
		isc_buffer_putmem(buf, client->cookie, 8);
		isc_buffer_putuint32(buf, nonce);
		isc_buffer_putuint32(buf, when);
		memmove(input, cp, 16);
		isc_aes128_crypt(secret, input, digest);
		for (i = 0; i < 8; i++) {
			input[i] = digest[i] ^ digest[i + 8];
		}
		isc_netaddr_fromsockaddr(&netaddr, &client->peeraddr);
		switch (netaddr.family) {
		case AF_INET:
			cp = (unsigned char *)&netaddr.type.in;
			memmove(input + 8, cp, 4);
			memset(input + 12, 0, 4);
			isc_aes128_crypt(secret, input, digest);
			break;
		case AF_INET6:
			cp = (unsigned char *)&netaddr.type.in6;
			memmove(input + 8, cp, 16);
			isc_aes128_crypt(secret, input, digest);
			for (i = 0; i < 8; i++) {
				input[i + 8] = digest[i] ^ digest[i + 8];
			}
			isc_aes128_crypt(client->sctx->secret, input + 8,
					 digest);
			break;
		default:
			INSIST(0);
			ISC_UNREACHABLE();
		}
		for (i = 0; i < 8; i++) {
			digest[i] ^= digest[i + 8];
		}
		isc_buffer_putmem(buf, digest, 8);
		break;
	}

	default:
		INSIST(0);
		ISC_UNREACHABLE();
	}
}