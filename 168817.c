static BOOL rdg_skip_seed_payload(rdpTls* tls, SSIZE_T lastResponseLength)
{
	BYTE seed_payload[10];
	const size_t size = sizeof(seed_payload);

	assert(size < SSIZE_MAX);

	/* Per [MS-TSGU] 3.3.5.1 step 4, after final OK response RDG server sends
	 * random "seed" payload of limited size. In practice it's 10 bytes.
	 */
	if (lastResponseLength < (SSIZE_T)size)
	{
		if (!rdg_read_all(tls, seed_payload, size - lastResponseLength))
		{
			return FALSE;
		}
	}

	return TRUE;
}