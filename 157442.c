void bnxt_re_query_fw_str(struct ib_device *ibdev, char *str)
{
	struct bnxt_re_dev *rdev = to_bnxt_re_dev(ibdev, ibdev);

	snprintf(str, IB_FW_VERSION_NAME_MAX, "%d.%d.%d.%d",
		 rdev->dev_attr.fw_ver[0], rdev->dev_attr.fw_ver[1],
		 rdev->dev_attr.fw_ver[2], rdev->dev_attr.fw_ver[3]);
}