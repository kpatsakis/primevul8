static struct sctp_chunk *_sctp_make_chunk(const struct sctp_association *asoc,
					    __u8 type, __u8 flags, int paylen)
{
	struct sctp_chunk *retval;
	sctp_chunkhdr_t *chunk_hdr;
	struct sk_buff *skb;
	struct sock *sk;

	/* No need to allocate LL here, as this is only a chunk. */
	skb = alloc_skb(WORD_ROUND(sizeof(sctp_chunkhdr_t) + paylen),
			GFP_ATOMIC);
	if (!skb)
		goto nodata;

	/* Make room for the chunk header.  */
	chunk_hdr = (sctp_chunkhdr_t *)skb_put(skb, sizeof(sctp_chunkhdr_t));
	chunk_hdr->type	  = type;
	chunk_hdr->flags  = flags;
	chunk_hdr->length = htons(sizeof(sctp_chunkhdr_t));

	sk = asoc ? asoc->base.sk : NULL;
	retval = sctp_chunkify(skb, asoc, sk);
	if (!retval) {
		kfree_skb(skb);
		goto nodata;
	}

	retval->chunk_hdr = chunk_hdr;
	retval->chunk_end = ((__u8 *)chunk_hdr) + sizeof(struct sctp_chunkhdr);

	/* Determine if the chunk needs to be authenticated */
	if (sctp_auth_send_cid(type, asoc))
		retval->auth = 1;

	return retval;
nodata:
	return NULL;
}