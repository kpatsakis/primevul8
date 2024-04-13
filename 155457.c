static int amba_find_match(struct device *dev, void *data)
{
	struct find_data *d = data;
	struct amba_device *pcdev = to_amba_device(dev);
	int r;

	r = (pcdev->periphid & d->mask) == d->id;
	if (d->parent)
		r &= d->parent == dev->parent;
	if (d->busid)
		r &= strcmp(dev_name(dev), d->busid) == 0;

	if (r) {
		get_device(dev);
		d->dev = pcdev;
	}

	return r;
}