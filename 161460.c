void init_timer_key(struct timer_list *timer, unsigned int flags,
		    const char *name, struct lock_class_key *key)
{
	debug_init(timer);
	do_init_timer(timer, flags, name, key);
}