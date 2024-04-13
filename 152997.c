static int arc_emac_rx(struct net_device *ndev, int budget)
{
	struct arc_emac_priv *priv = netdev_priv(ndev);
	unsigned int work_done;

	for (work_done = 0; work_done < budget; work_done++) {
		unsigned int *last_rx_bd = &priv->last_rx_bd;
		struct net_device_stats *stats = &ndev->stats;
		struct buffer_state *rx_buff = &priv->rx_buff[*last_rx_bd];
		struct arc_emac_bd *rxbd = &priv->rxbd[*last_rx_bd];
		unsigned int pktlen, info = le32_to_cpu(rxbd->info);
		struct sk_buff *skb;
		dma_addr_t addr;

		if (unlikely((info & OWN_MASK) == FOR_EMAC))
			break;

		/* Make a note that we saw a packet at this BD.
		 * So next time, driver starts from this + 1
		 */
		*last_rx_bd = (*last_rx_bd + 1) % RX_BD_NUM;

		if (unlikely((info & FIRST_OR_LAST_MASK) !=
			     FIRST_OR_LAST_MASK)) {
			/* We pre-allocate buffers of MTU size so incoming
			 * packets won't be split/chained.
			 */
			if (net_ratelimit())
				netdev_err(ndev, "incomplete packet received\n");

			/* Return ownership to EMAC */
			rxbd->info = cpu_to_le32(FOR_EMAC | EMAC_BUFFER_SIZE);
			stats->rx_errors++;
			stats->rx_length_errors++;
			continue;
		}

		pktlen = info & LEN_MASK;
		stats->rx_packets++;
		stats->rx_bytes += pktlen;
		skb = rx_buff->skb;
		skb_put(skb, pktlen);
		skb->dev = ndev;
		skb->protocol = eth_type_trans(skb, ndev);

		dma_unmap_single(&ndev->dev, dma_unmap_addr(rx_buff, addr),
				 dma_unmap_len(rx_buff, len), DMA_FROM_DEVICE);

		/* Prepare the BD for next cycle */
		rx_buff->skb = netdev_alloc_skb_ip_align(ndev,
							 EMAC_BUFFER_SIZE);
		if (unlikely(!rx_buff->skb)) {
			stats->rx_errors++;
			/* Because receive_skb is below, increment rx_dropped */
			stats->rx_dropped++;
			continue;
		}

		/* receive_skb only if new skb was allocated to avoid holes */
		netif_receive_skb(skb);

		addr = dma_map_single(&ndev->dev, (void *)rx_buff->skb->data,
				      EMAC_BUFFER_SIZE, DMA_FROM_DEVICE);
		if (dma_mapping_error(&ndev->dev, addr)) {
			if (net_ratelimit())
				netdev_err(ndev, "cannot dma map\n");
			dev_kfree_skb(rx_buff->skb);
			stats->rx_errors++;
			continue;
		}
		dma_unmap_addr_set(rx_buff, addr, addr);
		dma_unmap_len_set(rx_buff, len, EMAC_BUFFER_SIZE);

		rxbd->data = cpu_to_le32(addr);

		/* Make sure pointer to data buffer is set */
		wmb();

		/* Return ownership to EMAC */
		rxbd->info = cpu_to_le32(FOR_EMAC | EMAC_BUFFER_SIZE);
	}

	return work_done;
}