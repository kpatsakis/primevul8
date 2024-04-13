static inline void unlock_mount_hash(void)
{
	write_sequnlock(&mount_lock);
}