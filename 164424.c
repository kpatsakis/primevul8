static int evdev_handle_get_val(struct evdev_client *client,
				struct input_dev *dev, unsigned int type,
				unsigned long *bits, unsigned int maxbit,
				unsigned int maxlen, void __user *p,
				int compat)
{
	int ret;
	unsigned long *mem;
	size_t len;

	len = BITS_TO_LONGS(maxbit) * sizeof(unsigned long);
	mem = kmalloc(len, GFP_KERNEL);
	if (!mem)
		return -ENOMEM;

	spin_lock_irq(&dev->event_lock);
	spin_lock(&client->buffer_lock);

	memcpy(mem, bits, len);

	spin_unlock(&dev->event_lock);

	__evdev_flush_queue(client, type);

	spin_unlock_irq(&client->buffer_lock);

	ret = bits_to_user(mem, maxbit, maxlen, p, compat);
	if (ret < 0)
		evdev_queue_syn_dropped(client);

	kfree(mem);

	return ret;
}