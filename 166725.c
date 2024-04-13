void mlx5_error_sw_reset(struct mlx5_core_dev *dev)
{
	unsigned long end, delay_ms = MLX5_FW_RESET_WAIT_MS;
	int lock = -EBUSY;

	mutex_lock(&dev->intf_state_mutex);
	if (dev->state != MLX5_DEVICE_STATE_INTERNAL_ERROR)
		goto unlock;

	mlx5_core_err(dev, "start\n");

	if (check_fatal_sensors(dev) == MLX5_SENSOR_FW_SYND_RFR) {
		/* Get cr-dump and reset FW semaphore */
		lock = lock_sem_sw_reset(dev, true);

		if (lock == -EBUSY) {
			delay_ms = MLX5_CRDUMP_WAIT_MS;
			goto recover_from_sw_reset;
		}
		/* Execute SW reset */
		reset_fw_if_needed(dev);
	}

recover_from_sw_reset:
	/* Recover from SW reset */
	end = jiffies + msecs_to_jiffies(delay_ms);
	do {
		if (mlx5_get_nic_state(dev) == MLX5_NIC_IFC_DISABLED)
			break;

		cond_resched();
	} while (!time_after(jiffies, end));

	if (mlx5_get_nic_state(dev) != MLX5_NIC_IFC_DISABLED) {
		dev_err(&dev->pdev->dev, "NIC IFC still %d after %lums.\n",
			mlx5_get_nic_state(dev), delay_ms);
	}

	/* Release FW semaphore if you are the lock owner */
	if (!lock)
		lock_sem_sw_reset(dev, false);

	mlx5_core_err(dev, "end\n");

unlock:
	mutex_unlock(&dev->intf_state_mutex);
}