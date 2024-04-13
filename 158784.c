struct sctp_chunk *sctp_make_ecne(const struct sctp_association *asoc,
			     const __u32 lowest_tsn)
{
	struct sctp_chunk *retval;
	sctp_ecnehdr_t ecne;

	ecne.lowest_tsn = htonl(lowest_tsn);
	retval = sctp_make_control(asoc, SCTP_CID_ECN_ECNE, 0,
				   sizeof(sctp_ecnehdr_t));
	if (!retval)
		goto nodata;
	retval->subh.ecne_hdr =
		sctp_addto_chunk(retval, sizeof(ecne), &ecne);

nodata:
	return retval;
}