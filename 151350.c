ssize_t tpm_show_durations(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);

	return sprintf(buf, "%d %d %d [%s]\n",
		       jiffies_to_usecs(chip->vendor.duration[TPM_SHORT]),
		       jiffies_to_usecs(chip->vendor.duration[TPM_MEDIUM]),
		       jiffies_to_usecs(chip->vendor.duration[TPM_LONG]),
		       chip->vendor.duration_adjusted
		       ? "adjusted" : "original");
}