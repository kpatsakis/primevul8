
static clockid_t io_timeout_get_clock(struct io_timeout_data *data)
{
	switch (data->flags & IORING_TIMEOUT_CLOCK_MASK) {
	case IORING_TIMEOUT_BOOTTIME:
		return CLOCK_BOOTTIME;
	case IORING_TIMEOUT_REALTIME:
		return CLOCK_REALTIME;
	default:
		/* can't happen, vetted at prep time */
		WARN_ON_ONCE(1);
		fallthrough;
	case 0:
		return CLOCK_MONOTONIC;
	}