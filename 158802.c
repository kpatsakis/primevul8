struct sctp_chunk *sctp_make_cwr(const struct sctp_association *asoc,
			    const __u32 lowest_tsn,
			    const struct sctp_chunk *chunk)
{
	struct sctp_chunk *retval;
	sctp_cwrhdr_t cwr;

	cwr.lowest_tsn = htonl(lowest_tsn);
	retval = sctp_make_control(asoc, SCTP_CID_ECN_CWR, 0,
				   sizeof(sctp_cwrhdr_t));

	if (!retval)
		goto nodata;

	retval->subh.ecn_cwr_hdr =
		sctp_addto_chunk(retval, sizeof(cwr), &cwr);

	/* RFC 2960 6.4 Multi-homed SCTP Endpoints
	 *
	 * An endpoint SHOULD transmit reply chunks (e.g., SACK,
	 * HEARTBEAT ACK, * etc.) to the same destination transport
	 * address from which it * received the DATA or control chunk
	 * to which it is replying.
	 *
	 * [Report a reduced congestion window back to where the ECNE
	 * came from.]
	 */
	if (chunk)
		retval->transport = chunk->transport;

nodata:
	return retval;
}