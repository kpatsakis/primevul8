void rtnl_af_register(struct rtnl_af_ops *ops)
{
	rtnl_lock();
	list_add_tail(&ops->list, &rtnl_af_ops);
	rtnl_unlock();
}