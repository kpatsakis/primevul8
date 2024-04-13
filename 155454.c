amba_find_device(const char *busid, struct device *parent, unsigned int id,
		 unsigned int mask)
{
	struct find_data data;

	data.dev = NULL;
	data.parent = parent;
	data.busid = busid;
	data.id = id;
	data.mask = mask;

	bus_for_each_dev(&amba_bustype, NULL, &data, amba_find_match);

	return data.dev;
}