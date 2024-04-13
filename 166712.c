static u32 check_fatal_sensors(struct mlx5_core_dev *dev)
{
	if (sensor_pci_not_working(dev))
		return MLX5_SENSOR_PCI_COMM_ERR;
	if (pci_channel_offline(dev->pdev))
		return MLX5_SENSOR_PCI_ERR;
	if (mlx5_get_nic_state(dev) == MLX5_NIC_IFC_DISABLED)
		return MLX5_SENSOR_NIC_DISABLED;
	if (mlx5_get_nic_state(dev) == MLX5_NIC_IFC_SW_RESET)
		return MLX5_SENSOR_NIC_SW_RESET;
	if (sensor_fw_synd_rfr(dev))
		return MLX5_SENSOR_FW_SYND_RFR;

	return MLX5_SENSOR_NO_ERR;
}