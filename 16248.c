static int __io_remove_buffers(struct io_ring_ctx *ctx,
			       struct io_buffer_list *bl, unsigned nbufs)
{
	unsigned i = 0;

	/* shouldn't happen */
	if (!nbufs)
		return 0;

	/* the head kbuf is the list itself */
	while (!list_empty(&bl->buf_list)) {
		struct io_buffer *nxt;

		nxt = list_first_entry(&bl->buf_list, struct io_buffer, list);
		list_del(&nxt->list);
		if (++i == nbufs)
			return i;
		cond_resched();
	}
	i++;

	return i;
}