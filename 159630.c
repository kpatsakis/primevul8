static int snd_timer_s_stop(struct snd_timer * timer)
{
	struct snd_timer_system_private *priv;
	unsigned long jiff;

	priv = (struct snd_timer_system_private *) timer->private_data;
	del_timer(&priv->tlist);
	jiff = jiffies;
	if (time_before(jiff, priv->last_expires))
		timer->sticks = priv->last_expires - jiff;
	else
		timer->sticks = 1;
	priv->correction = 0;
	return 0;
}