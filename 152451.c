static void queue_cleanup(struct list_head *list)
{
	struct read_buffer *rb;

	while (!list_empty(list)) {
		rb = list_entry(list->next, struct read_buffer, list);
		list_del(list->next);
		kfree(rb);
	}
}