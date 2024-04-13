void mlx5_health_flush(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;

	flush_workqueue(health->wq);
}