ssize_t tpm_show_pubek(struct device *dev, struct device_attribute *attr,
		       char *buf)
{
	u8 *data;
	struct tpm_cmd_t tpm_cmd;
	ssize_t err;
	int i, rc;
	char *str = buf;

	struct tpm_chip *chip = dev_get_drvdata(dev);

	tpm_cmd.header.in = tpm_readpubek_header;
	err = transmit_cmd(chip, &tpm_cmd, READ_PUBEK_RESULT_SIZE,
			"attempting to read the PUBEK");
	if (err)
		goto out;

	/* 
	   ignore header 10 bytes
	   algorithm 32 bits (1 == RSA )
	   encscheme 16 bits
	   sigscheme 16 bits
	   parameters (RSA 12->bytes: keybit, #primes, expbit)  
	   keylenbytes 32 bits
	   256 byte modulus
	   ignore checksum 20 bytes
	 */
	data = tpm_cmd.params.readpubek_out_buffer;
	str +=
	    sprintf(str,
		    "Algorithm: %02X %02X %02X %02X\n"
		    "Encscheme: %02X %02X\n"
		    "Sigscheme: %02X %02X\n"
		    "Parameters: %02X %02X %02X %02X "
		    "%02X %02X %02X %02X "
		    "%02X %02X %02X %02X\n"
		    "Modulus length: %d\n"
		    "Modulus:\n",
		    data[0], data[1], data[2], data[3],
		    data[4], data[5],
		    data[6], data[7],
		    data[12], data[13], data[14], data[15],
		    data[16], data[17], data[18], data[19],
		    data[20], data[21], data[22], data[23],
		    be32_to_cpu(*((__be32 *) (data + 24))));

	for (i = 0; i < 256; i++) {
		str += sprintf(str, "%02X ", data[i + 28]);
		if ((i + 1) % 16 == 0)
			str += sprintf(str, "\n");
	}
out:
	rc = str - buf;
	return rc;
}