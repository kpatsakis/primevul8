static void *sctp_addto_chunk_fixed(struct sctp_chunk *chunk,
				    int len, const void *data)
{
	if (skb_tailroom(chunk->skb) >= len)
		return sctp_addto_chunk(chunk, len, data);
	else
		return NULL;
}