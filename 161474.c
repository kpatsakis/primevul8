static inline void init_timer_on_stack_key(struct timer_list *timer,
					   unsigned int flags, const char *name,
					   struct lock_class_key *key)
{
	init_timer_key(timer, flags, name, key);
}