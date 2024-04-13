void mlx5_start_health_poll(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;

	timer_setup(&health->timer, poll_health, 0);
	health->fatal_error = MLX5_SENSOR_NO_ERR;
	clear_bit(MLX5_DROP_NEW_HEALTH_WORK, &health->flags);
	health->health = &dev->iseg->health;
	health->health_counter = &dev->iseg->health_counter;

	health->timer.expires = round_jiffies(jiffies + MLX5_HEALTH_POLL_INTERVAL);
	add_timer(&health->timer);
}