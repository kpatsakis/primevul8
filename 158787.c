struct sctp_chunk *sctp_make_shutdown(const struct sctp_association *asoc,
				      const struct sctp_chunk *chunk)
{
	struct sctp_chunk *retval;
	sctp_shutdownhdr_t shut;
	__u32 ctsn;

	ctsn = sctp_tsnmap_get_ctsn(&asoc->peer.tsn_map);
	shut.cum_tsn_ack = htonl(ctsn);

	retval = sctp_make_control(asoc, SCTP_CID_SHUTDOWN, 0,
				   sizeof(sctp_shutdownhdr_t));
	if (!retval)
		goto nodata;

	retval->subh.shutdown_hdr =
		sctp_addto_chunk(retval, sizeof(shut), &shut);

	if (chunk)
		retval->transport = chunk->transport;
nodata:
	return retval;
}