int bnxt_re_modify_device(struct ib_device *ibdev,
			  int device_modify_mask,
			  struct ib_device_modify *device_modify)
{
	switch (device_modify_mask) {
	case IB_DEVICE_MODIFY_SYS_IMAGE_GUID:
		/* Modify the GUID requires the modification of the GID table */
		/* GUID should be made as READ-ONLY */
		break;
	case IB_DEVICE_MODIFY_NODE_DESC:
		/* Node Desc should be made as READ-ONLY */
		break;
	default:
		break;
	}
	return 0;
}