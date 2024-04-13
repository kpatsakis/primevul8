int mod_timer_pending(struct timer_list *timer, unsigned long expires)
{
	return __mod_timer(timer, expires, true);
}