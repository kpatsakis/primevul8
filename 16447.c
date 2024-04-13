
static inline bool io_sqd_events_pending(struct io_sq_data *sqd)
{
	return READ_ONCE(sqd->state);