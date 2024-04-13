static int queue_reply(struct list_head *queue, const void *data, size_t len)
{
	struct read_buffer *rb;

	if (len == 0)
		return 0;
	if (len > XENSTORE_PAYLOAD_MAX)
		return -EINVAL;

	rb = kmalloc(sizeof(*rb) + len, GFP_KERNEL);
	if (rb == NULL)
		return -ENOMEM;

	rb->cons = 0;
	rb->len = len;

	memcpy(rb->msg, data, len);

	list_add_tail(&rb->list, queue);
	return 0;
}