void mlx5_enter_error_state(struct mlx5_core_dev *dev, bool force)
{
	mutex_lock(&dev->intf_state_mutex);
	if (dev->state == MLX5_DEVICE_STATE_INTERNAL_ERROR)
		goto unlock;
	if (dev->state == MLX5_DEVICE_STATE_UNINITIALIZED) {
		dev->state = MLX5_DEVICE_STATE_INTERNAL_ERROR;
		goto unlock;
	}

	if (check_fatal_sensors(dev) || force) {
		dev->state = MLX5_DEVICE_STATE_INTERNAL_ERROR;
		mlx5_cmd_flush(dev);
	}

	mlx5_notifier_call_chain(dev->priv.events, MLX5_DEV_EVENT_SYS_ERROR, (void *)1);
unlock:
	mutex_unlock(&dev->intf_state_mutex);
}