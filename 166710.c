static void poll_health(struct timer_list *t)
{
	struct mlx5_core_dev *dev = from_timer(dev, t, priv.health.timer);
	struct mlx5_core_health *health = &dev->priv.health;
	struct health_buffer __iomem *h = health->health;
	u32 fatal_error;
	u8 prev_synd;
	u32 count;

	if (dev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR)
		goto out;

	fatal_error = check_fatal_sensors(dev);

	if (fatal_error && !health->fatal_error) {
		mlx5_core_err(dev, "Fatal error %u detected\n", fatal_error);
		dev->priv.health.fatal_error = fatal_error;
		print_health_info(dev);
		mlx5_trigger_health_work(dev);
		goto out;
	}

	count = ioread32be(health->health_counter);
	if (count == health->prev)
		++health->miss_counter;
	else
		health->miss_counter = 0;

	health->prev = count;
	if (health->miss_counter == MAX_MISSES) {
		mlx5_core_err(dev, "device's health compromised - reached miss count\n");
		print_health_info(dev);
		queue_work(health->wq, &health->report_work);
	}

	prev_synd = health->synd;
	health->synd = ioread8(&h->synd);
	if (health->synd && health->synd != prev_synd)
		queue_work(health->wq, &health->report_work);

out:
	mod_timer(&health->timer, get_next_poll_jiffies());
}