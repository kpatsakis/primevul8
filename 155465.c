static int amba_uevent(struct device *dev, struct kobj_uevent_env *env)
{
	struct amba_device *pcdev = to_amba_device(dev);
	int retval = 0;

	retval = add_uevent_var(env, "AMBA_ID=%08x", pcdev->periphid);
	if (retval)
		return retval;

	retval = add_uevent_var(env, "MODALIAS=amba:d%08X", pcdev->periphid);
	return retval;
}