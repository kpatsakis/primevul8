ssize_t tpm_show_timeouts(struct device *dev, struct device_attribute *attr,
			  char *buf)
{
	struct tpm_chip *chip = dev_get_drvdata(dev);

	return sprintf(buf, "%d %d %d %d [%s]\n",
		       jiffies_to_usecs(chip->vendor.timeout_a),
		       jiffies_to_usecs(chip->vendor.timeout_b),
		       jiffies_to_usecs(chip->vendor.timeout_c),
		       jiffies_to_usecs(chip->vendor.timeout_d),
		       chip->vendor.timeout_adjusted
		       ? "adjusted" : "original");
}