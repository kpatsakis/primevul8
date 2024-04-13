int snd_power_wait(struct snd_card *card, unsigned int power_state)
{
	wait_queue_entry_t wait;
	int result = 0;

	/* fastpath */
	if (snd_power_get_state(card) == power_state)
		return 0;
	init_waitqueue_entry(&wait, current);
	add_wait_queue(&card->power_sleep, &wait);
	while (1) {
		if (card->shutdown) {
			result = -ENODEV;
			break;
		}
		if (snd_power_get_state(card) == power_state)
			break;
		set_current_state(TASK_UNINTERRUPTIBLE);
		schedule_timeout(30 * HZ);
	}
	remove_wait_queue(&card->power_sleep, &wait);
	return result;
}