static void arc_emac_set_address_internal(struct net_device *ndev)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);
	unsigned int addr_low, addr_hi;

	addr_low = le32_to_cpu(*(__le32 *) &ndev->dev_addr[0]);
	addr_hi = le16_to_cpu(*(__le16 *) &ndev->dev_addr[4]);

	arc_reg_set(priv, R_ADDRL, addr_low);
	arc_reg_set(priv, R_ADDRH, addr_hi);
}