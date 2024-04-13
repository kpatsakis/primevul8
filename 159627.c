static void snd_timer_user_interrupt(struct snd_timer_instance *timeri,
				     unsigned long resolution,
				     unsigned long ticks)
{
	struct snd_timer_user *tu = timeri->callback_data;
	struct snd_timer_read *r;
	int prev;

	spin_lock(&tu->qlock);
	if (tu->qused > 0) {
		prev = tu->qtail == 0 ? tu->queue_size - 1 : tu->qtail - 1;
		r = &tu->queue[prev];
		if (r->resolution == resolution) {
			r->ticks += ticks;
			goto __wake;
		}
	}
	if (tu->qused >= tu->queue_size) {
		tu->overrun++;
	} else {
		r = &tu->queue[tu->qtail++];
		tu->qtail %= tu->queue_size;
		r->resolution = resolution;
		r->ticks = ticks;
		tu->qused++;
	}
      __wake:
	spin_unlock(&tu->qlock);
	kill_fasync(&tu->fasync, SIGIO, POLL_IN);
	wake_up(&tu->qchange_sleep);
}