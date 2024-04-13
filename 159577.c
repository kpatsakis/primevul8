static void snd_timer_request(struct snd_timer_id *tid)
{
	switch (tid->dev_class) {
	case SNDRV_TIMER_CLASS_GLOBAL:
		if (tid->device < timer_limit)
			request_module("snd-timer-%i", tid->device);
		break;
	case SNDRV_TIMER_CLASS_CARD:
	case SNDRV_TIMER_CLASS_PCM:
		if (tid->card < snd_ecards_limit)
			request_module("snd-card-%i", tid->card);
		break;
	default:
		break;
	}
}