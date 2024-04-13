void mlx5_drain_health_wq(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;
	unsigned long flags;

	spin_lock_irqsave(&health->wq_lock, flags);
	set_bit(MLX5_DROP_NEW_HEALTH_WORK, &health->flags);
	spin_unlock_irqrestore(&health->wq_lock, flags);
	cancel_work_sync(&health->report_work);
	cancel_work_sync(&health->fatal_report_work);
}