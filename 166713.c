static bool sensor_pci_not_working(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;
	struct health_buffer __iomem *h = health->health;

	/* Offline PCI reads return 0xffffffff */
	return (ioread32be(&h->fw_ver) == 0xffffffff);
}