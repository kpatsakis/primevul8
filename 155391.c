static long vhost_net_set_owner(struct vhost_net *n)
{
	int r;

	mutex_lock(&n->dev.mutex);
	if (vhost_dev_has_owner(&n->dev)) {
		r = -EBUSY;
		goto out;
	}
	r = vhost_net_set_ubuf_info(n);
	if (r)
		goto out;
	r = vhost_dev_set_owner(&n->dev);
	if (r)
		vhost_net_clear_ubuf_info(n);
	vhost_net_flush(n);
out:
	mutex_unlock(&n->dev.mutex);
	return r;
}