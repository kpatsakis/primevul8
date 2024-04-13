static struct sctp_chunk *sctp_make_control(const struct sctp_association *asoc,
					    __u8 type, __u8 flags, int paylen)
{
	struct sctp_chunk *chunk = _sctp_make_chunk(asoc, type, flags, paylen);

	if (chunk)
		sctp_control_set_owner_w(chunk);

	return chunk;
}