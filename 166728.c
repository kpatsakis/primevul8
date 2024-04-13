void mlx5_trigger_health_work(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;
	unsigned long flags;

	spin_lock_irqsave(&health->wq_lock, flags);
	if (!test_bit(MLX5_DROP_NEW_HEALTH_WORK, &health->flags))
		queue_work(health->wq, &health->fatal_report_work);
	else
		mlx5_core_err(dev, "new health works are not permitted at this stage\n");
	spin_unlock_irqrestore(&health->wq_lock, flags);
}