static void arc_emac_poll_controller(struct net_device *dev)
{
	disable_irq(dev->irq);
	arc_emac_intr(dev->irq, dev);
	enable_irq(dev->irq);
}