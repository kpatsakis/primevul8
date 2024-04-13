static int mlx5_health_try_recover(struct mlx5_core_dev *dev)
{
	unsigned long end;

	mlx5_core_warn(dev, "handling bad device here\n");
	mlx5_handle_bad_state(dev);
	end = jiffies + msecs_to_jiffies(MLX5_RECOVERY_WAIT_MSECS);
	while (sensor_pci_not_working(dev)) {
		if (time_after(jiffies, end)) {
			mlx5_core_err(dev,
				      "health recovery flow aborted, PCI reads still not working\n");
			return -EIO;
		}
		msleep(100);
	}

	mlx5_core_err(dev, "starting health recovery flow\n");
	mlx5_recover_device(dev);
	if (!test_bit(MLX5_INTERFACE_STATE_UP, &dev->intf_state) ||
	    check_fatal_sensors(dev)) {
		mlx5_core_err(dev, "health recovery failed\n");
		return -EIO;
	}
	return 0;
}