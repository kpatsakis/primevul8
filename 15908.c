static inline void lock_mount_hash(void)
{
	write_seqlock(&mount_lock);
}