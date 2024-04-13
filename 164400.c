static unsigned int evdev_compute_buffer_size(struct input_dev *dev)
{
	unsigned int n_events =
		max(dev->hint_events_per_packet * EVDEV_BUF_PACKETS,
		    EVDEV_MIN_BUFFER_SIZE);

	return roundup_pow_of_two(n_events);
}