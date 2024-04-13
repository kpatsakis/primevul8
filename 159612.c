int snd_timer_global_new(char *id, int device, struct snd_timer **rtimer)
{
	struct snd_timer_id tid;

	tid.dev_class = SNDRV_TIMER_CLASS_GLOBAL;
	tid.dev_sclass = SNDRV_TIMER_SCLASS_NONE;
	tid.card = -1;
	tid.device = device;
	tid.subdevice = 0;
	return snd_timer_new(NULL, id, &tid, rtimer);
}