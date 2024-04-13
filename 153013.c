add_one_guest_page(struct virtio_net *dev, uint64_t guest_phys_addr,
		   uint64_t host_iova, uint64_t host_user_addr, uint64_t size)
{
	struct guest_page *page, *last_page;
	struct guest_page *old_pages;

	if (dev->nr_guest_pages == dev->max_guest_pages) {
		dev->max_guest_pages *= 2;
		old_pages = dev->guest_pages;
		dev->guest_pages = rte_realloc(dev->guest_pages,
					dev->max_guest_pages * sizeof(*page),
					RTE_CACHE_LINE_SIZE);
		if (dev->guest_pages == NULL) {
			VHOST_LOG_CONFIG(ERR, "cannot realloc guest_pages\n");
			rte_free(old_pages);
			return -1;
		}
	}

	if (dev->nr_guest_pages > 0) {
		last_page = &dev->guest_pages[dev->nr_guest_pages - 1];
		/* merge if the two pages are continuous */
		if (host_iova == last_page->host_iova + last_page->size &&
		    guest_phys_addr == last_page->guest_phys_addr + last_page->size &&
		    host_user_addr == last_page->host_user_addr + last_page->size) {
			last_page->size += size;
			return 0;
		}
	}

	page = &dev->guest_pages[dev->nr_guest_pages++];
	page->guest_phys_addr = guest_phys_addr;
	page->host_iova  = host_iova;
	page->host_user_addr = host_user_addr;
	page->size = size;

	return 0;
}