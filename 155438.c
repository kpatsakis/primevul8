void amba_driver_unregister(struct amba_driver *drv)
{
	driver_unregister(&drv->drv);
}