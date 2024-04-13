amba_lookup(const struct amba_id *table, struct amba_device *dev)
{
	int ret = 0;

	while (table->mask) {
		ret = (dev->periphid & table->mask) == table->id;
		if (ret)
			break;
		table++;
	}

	return ret ? table : NULL;
}