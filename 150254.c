static ssize_t show_abi_version(struct device *dev,
				struct device_attribute *attr,
				char *buf)
{
	return sprintf(buf, "%d\n", RDMA_USER_CM_ABI_VERSION);
}