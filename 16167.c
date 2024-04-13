
static inline void io_poll_remove_entry(struct io_poll_iocb *poll)
{
	struct wait_queue_head *head = smp_load_acquire(&poll->head);

	if (head) {
		spin_lock_irq(&head->lock);
		list_del_init(&poll->wait.entry);
		poll->head = NULL;
		spin_unlock_irq(&head->lock);
	}