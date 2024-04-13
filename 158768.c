static sctp_cookie_param_t *sctp_pack_cookie(const struct sctp_endpoint *ep,
				      const struct sctp_association *asoc,
				      const struct sctp_chunk *init_chunk,
				      int *cookie_len,
				      const __u8 *raw_addrs, int addrs_len)
{
	sctp_cookie_param_t *retval;
	struct sctp_signed_cookie *cookie;
	struct scatterlist sg;
	int headersize, bodysize;

	/* Header size is static data prior to the actual cookie, including
	 * any padding.
	 */
	headersize = sizeof(sctp_paramhdr_t) +
		     (sizeof(struct sctp_signed_cookie) -
		      sizeof(struct sctp_cookie));
	bodysize = sizeof(struct sctp_cookie)
		+ ntohs(init_chunk->chunk_hdr->length) + addrs_len;

	/* Pad out the cookie to a multiple to make the signature
	 * functions simpler to write.
	 */
	if (bodysize % SCTP_COOKIE_MULTIPLE)
		bodysize += SCTP_COOKIE_MULTIPLE
			- (bodysize % SCTP_COOKIE_MULTIPLE);
	*cookie_len = headersize + bodysize;

	/* Clear this memory since we are sending this data structure
	 * out on the network.
	 */
	retval = kzalloc(*cookie_len, GFP_ATOMIC);
	if (!retval)
		goto nodata;

	cookie = (struct sctp_signed_cookie *) retval->body;

	/* Set up the parameter header.  */
	retval->p.type = SCTP_PARAM_STATE_COOKIE;
	retval->p.length = htons(*cookie_len);

	/* Copy the cookie part of the association itself.  */
	cookie->c = asoc->c;
	/* Save the raw address list length in the cookie. */
	cookie->c.raw_addr_list_len = addrs_len;

	/* Remember PR-SCTP capability. */
	cookie->c.prsctp_capable = asoc->peer.prsctp_capable;

	/* Save adaptation indication in the cookie. */
	cookie->c.adaptation_ind = asoc->peer.adaptation_ind;

	/* Set an expiration time for the cookie.  */
	cookie->c.expiration = ktime_add(asoc->cookie_life,
					 ktime_get());

	/* Copy the peer's init packet.  */
	memcpy(&cookie->c.peer_init[0], init_chunk->chunk_hdr,
	       ntohs(init_chunk->chunk_hdr->length));

	/* Copy the raw local address list of the association. */
	memcpy((__u8 *)&cookie->c.peer_init[0] +
	       ntohs(init_chunk->chunk_hdr->length), raw_addrs, addrs_len);

	if (sctp_sk(ep->base.sk)->hmac) {
		struct hash_desc desc;

		/* Sign the message.  */
		sg_init_one(&sg, &cookie->c, bodysize);
		desc.tfm = sctp_sk(ep->base.sk)->hmac;
		desc.flags = 0;

		if (crypto_hash_setkey(desc.tfm, ep->secret_key,
				       sizeof(ep->secret_key)) ||
		    crypto_hash_digest(&desc, &sg, bodysize, cookie->signature))
			goto free_cookie;
	}

	return retval;

free_cookie:
	kfree(retval);
nodata:
	*cookie_len = 0;
	return NULL;
}