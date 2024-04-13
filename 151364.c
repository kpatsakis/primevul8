void tpm_get_timeouts(struct tpm_chip *chip)
{
	struct tpm_cmd_t tpm_cmd;
	struct timeout_t *timeout_cap;
	struct duration_t *duration_cap;
	ssize_t rc;
	u32 timeout;
	unsigned int scale = 1;

	tpm_cmd.header.in = tpm_getcap_header;
	tpm_cmd.params.getcap_in.cap = TPM_CAP_PROP;
	tpm_cmd.params.getcap_in.subcap_size = cpu_to_be32(4);
	tpm_cmd.params.getcap_in.subcap = TPM_CAP_PROP_TIS_TIMEOUT;

	rc = transmit_cmd(chip, &tpm_cmd, TPM_INTERNAL_RESULT_SIZE,
			"attempting to determine the timeouts");
	if (rc)
		goto duration;

	if (be32_to_cpu(tpm_cmd.header.out.return_code) != 0 ||
	    be32_to_cpu(tpm_cmd.header.out.length)
	    != sizeof(tpm_cmd.header.out) + sizeof(u32) + 4 * sizeof(u32))
		return;

	timeout_cap = &tpm_cmd.params.getcap_out.cap.timeout;
	/* Don't overwrite default if value is 0 */
	timeout = be32_to_cpu(timeout_cap->a);
	if (timeout && timeout < 1000) {
		/* timeouts in msec rather usec */
		scale = 1000;
		chip->vendor.timeout_adjusted = true;
	}
	if (timeout)
		chip->vendor.timeout_a = usecs_to_jiffies(timeout * scale);
	timeout = be32_to_cpu(timeout_cap->b);
	if (timeout)
		chip->vendor.timeout_b = usecs_to_jiffies(timeout * scale);
	timeout = be32_to_cpu(timeout_cap->c);
	if (timeout)
		chip->vendor.timeout_c = usecs_to_jiffies(timeout * scale);
	timeout = be32_to_cpu(timeout_cap->d);
	if (timeout)
		chip->vendor.timeout_d = usecs_to_jiffies(timeout * scale);

duration:
	tpm_cmd.header.in = tpm_getcap_header;
	tpm_cmd.params.getcap_in.cap = TPM_CAP_PROP;
	tpm_cmd.params.getcap_in.subcap_size = cpu_to_be32(4);
	tpm_cmd.params.getcap_in.subcap = TPM_CAP_PROP_TIS_DURATION;

	rc = transmit_cmd(chip, &tpm_cmd, TPM_INTERNAL_RESULT_SIZE,
			"attempting to determine the durations");
	if (rc)
		return;

	if (be32_to_cpu(tpm_cmd.header.out.return_code) != 0 ||
	    be32_to_cpu(tpm_cmd.header.out.length)
	    != sizeof(tpm_cmd.header.out) + sizeof(u32) + 3 * sizeof(u32))
		return;

	duration_cap = &tpm_cmd.params.getcap_out.cap.duration;
	chip->vendor.duration[TPM_SHORT] =
	    usecs_to_jiffies(be32_to_cpu(duration_cap->tpm_short));
	chip->vendor.duration[TPM_MEDIUM] =
	    usecs_to_jiffies(be32_to_cpu(duration_cap->tpm_medium));
	chip->vendor.duration[TPM_LONG] =
	    usecs_to_jiffies(be32_to_cpu(duration_cap->tpm_long));

	/* The Broadcom BCM0102 chipset in a Dell Latitude D820 gets the above
	 * value wrong and apparently reports msecs rather than usecs. So we
	 * fix up the resulting too-small TPM_SHORT value to make things work.
	 * We also scale the TPM_MEDIUM and -_LONG values by 1000.
	 */
	if (chip->vendor.duration[TPM_SHORT] < (HZ / 100)) {
		chip->vendor.duration[TPM_SHORT] = HZ;
		chip->vendor.duration[TPM_MEDIUM] *= 1000;
		chip->vendor.duration[TPM_LONG] *= 1000;
		chip->vendor.duration_adjusted = true;
		dev_info(chip->dev, "Adjusting TPM timeout parameters.");
	}
}