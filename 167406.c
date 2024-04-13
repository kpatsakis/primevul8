static void hso_net_tx_timeout(struct net_device *net)
{
	struct hso_net *odev = netdev_priv(net);

	if (!odev)
		return;

	/* Tell syslog we are hosed. */
	dev_warn(&net->dev, "Tx timed out.\n");

	/* Tear the waiting frame off the list */
	if (odev->mux_bulk_tx_urb &&
	    (odev->mux_bulk_tx_urb->status == -EINPROGRESS))
		usb_unlink_urb(odev->mux_bulk_tx_urb);

	/* Update statistics */
	net->stats.tx_errors++;
}