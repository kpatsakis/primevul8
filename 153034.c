dump_guest_pages(struct virtio_net *dev)
{
	uint32_t i;
	struct guest_page *page;

	for (i = 0; i < dev->nr_guest_pages; i++) {
		page = &dev->guest_pages[i];

		VHOST_LOG_CONFIG(INFO, "(%s) guest physical page region %u\n",
				dev->ifname, i);
		VHOST_LOG_CONFIG(INFO, "(%s)\tguest_phys_addr: %" PRIx64 "\n",
				dev->ifname, page->guest_phys_addr);
		VHOST_LOG_CONFIG(INFO, "(%s)\thost_iova : %" PRIx64 "\n",
				dev->ifname, page->host_iova);
		VHOST_LOG_CONFIG(INFO, "(%s)\tsize           : %" PRIx64 "\n",
				dev->ifname, page->size);
	}
}