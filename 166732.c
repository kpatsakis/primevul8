static void print_health_info(struct mlx5_core_dev *dev)
{
	struct mlx5_core_health *health = &dev->priv.health;
	struct health_buffer __iomem *h = health->health;
	char fw_str[18];
	u32 fw;
	int i;

	/* If the syndrome is 0, the device is OK and no need to print buffer */
	if (!ioread8(&h->synd))
		return;

	for (i = 0; i < ARRAY_SIZE(h->assert_var); i++)
		mlx5_core_err(dev, "assert_var[%d] 0x%08x\n", i,
			      ioread32be(h->assert_var + i));

	mlx5_core_err(dev, "assert_exit_ptr 0x%08x\n",
		      ioread32be(&h->assert_exit_ptr));
	mlx5_core_err(dev, "assert_callra 0x%08x\n",
		      ioread32be(&h->assert_callra));
	sprintf(fw_str, "%d.%d.%d", fw_rev_maj(dev), fw_rev_min(dev), fw_rev_sub(dev));
	mlx5_core_err(dev, "fw_ver %s\n", fw_str);
	mlx5_core_err(dev, "hw_id 0x%08x\n", ioread32be(&h->hw_id));
	mlx5_core_err(dev, "irisc_index %d\n", ioread8(&h->irisc_index));
	mlx5_core_err(dev, "synd 0x%x: %s\n", ioread8(&h->synd),
		      hsynd_str(ioread8(&h->synd)));
	mlx5_core_err(dev, "ext_synd 0x%04x\n", ioread16be(&h->ext_synd));
	fw = ioread32be(&h->fw_ver);
	mlx5_core_err(dev, "raw fw_ver 0x%08x\n", fw);
}