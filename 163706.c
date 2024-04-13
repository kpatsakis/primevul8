int http_header_add_tail(struct http_msg *msg, struct hdr_idx *hdr_idx, const char *text)
{
	int bytes, len;

	len = strlen(text);
	bytes = buffer_insert_line2(msg->chn->buf, msg->chn->buf->p + msg->eoh, text, len);
	if (!bytes)
		return -1;
	http_msg_move_end(msg, bytes);
	return hdr_idx_add(len, 1, hdr_idx, hdr_idx->tail);
}