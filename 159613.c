static void snd_timer_user_append_to_tqueue(struct snd_timer_user *tu,
					    struct snd_timer_tread *tread)
{
	if (tu->qused >= tu->queue_size) {
		tu->overrun++;
	} else {
		memcpy(&tu->tqueue[tu->qtail++], tread, sizeof(*tread));
		tu->qtail %= tu->queue_size;
		tu->qused++;
	}
}