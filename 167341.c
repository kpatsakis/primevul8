static void handle_usb_error(int status, const char *function,
			     struct hso_device *hso_dev)
{
	char *explanation;

	switch (status) {
	case -ENODEV:
		explanation = "no device";
		break;
	case -ENOENT:
		explanation = "endpoint not enabled";
		break;
	case -EPIPE:
		explanation = "endpoint stalled";
		break;
	case -ENOSPC:
		explanation = "not enough bandwidth";
		break;
	case -ESHUTDOWN:
		explanation = "device disabled";
		break;
	case -EHOSTUNREACH:
		explanation = "device suspended";
		break;
	case -EINVAL:
	case -EAGAIN:
	case -EFBIG:
	case -EMSGSIZE:
		explanation = "internal error";
		break;
	case -EILSEQ:
	case -EPROTO:
	case -ETIME:
	case -ETIMEDOUT:
		explanation = "protocol error";
		if (hso_dev)
			usb_queue_reset_device(hso_dev->interface);
		break;
	default:
		explanation = "unknown status";
		break;
	}

	/* log a meaningful explanation of an USB status */
	hso_dbg(0x1, "%s: received USB status - %s (%d)\n",
		function, explanation, status);
}