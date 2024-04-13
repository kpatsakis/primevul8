static void snd_timer_s_function(unsigned long data)
{
	struct snd_timer *timer = (struct snd_timer *)data;
	struct snd_timer_system_private *priv = timer->private_data;
	unsigned long jiff = jiffies;
	if (time_after(jiff, priv->last_expires))
		priv->correction += (long)jiff - (long)priv->last_expires;
	snd_timer_interrupt(timer, (long)jiff - (long)priv->last_jiffies);
}