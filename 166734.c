int mlx5_health_init(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health;
	char *name;

	mlx5_fw_reporters_create(dev);

	health = &dev->priv.health;
	name = kmalloc(64, GFP_KERNEL);
	if (!name)
		goto out_err;

	strcpy(name, "mlx5_health");
	strcat(name, dev_name(dev->device));
	health->wq = create_singlethread_workqueue(name);
	kfree(name);
	if (!health->wq)
		goto out_err;
	spin_lock_init(&health->wq_lock);
	INIT_WORK(&health->fatal_report_work, mlx5_fw_fatal_reporter_err_work);
	INIT_WORK(&health->report_work, mlx5_fw_reporter_err_work);

	return 0;

out_err:
	mlx5_fw_reporters_destroy(dev);
	return -ENOMEM;
}