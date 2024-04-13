struct sctp_chunk *sctp_make_auth(const struct sctp_association *asoc)
{
	struct sctp_chunk *retval;
	struct sctp_hmac *hmac_desc;
	struct sctp_authhdr auth_hdr;
	__u8 *hmac;

	/* Get the first hmac that the peer told us to use */
	hmac_desc = sctp_auth_asoc_get_hmac(asoc);
	if (unlikely(!hmac_desc))
		return NULL;

	retval = sctp_make_control(asoc, SCTP_CID_AUTH, 0,
			hmac_desc->hmac_len + sizeof(sctp_authhdr_t));
	if (!retval)
		return NULL;

	auth_hdr.hmac_id = htons(hmac_desc->hmac_id);
	auth_hdr.shkey_id = htons(asoc->active_key_id);

	retval->subh.auth_hdr = sctp_addto_chunk(retval, sizeof(sctp_authhdr_t),
						&auth_hdr);

	hmac = skb_put(retval->skb, hmac_desc->hmac_len);
	memset(hmac, 0, hmac_desc->hmac_len);

	/* Adjust the chunk header to include the empty MAC */
	retval->chunk_hdr->length =
		htons(ntohs(retval->chunk_hdr->length) + hmac_desc->hmac_len);
	retval->chunk_end = skb_tail_pointer(retval->skb);

	return retval;
}