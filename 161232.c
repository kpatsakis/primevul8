long keyctl_instantiate_key_iov(key_serial_t id,
				const struct iovec __user *_payload_iov,
				unsigned ioc,
				key_serial_t ringid)
{
	struct iovec iovstack[UIO_FASTIOV], *iov = iovstack;
	long ret;

	if (!_payload_iov || !ioc)
		goto no_payload;

	ret = rw_copy_check_uvector(WRITE, _payload_iov, ioc,
				    ARRAY_SIZE(iovstack), iovstack, &iov);
	if (ret < 0)
		goto err;
	if (ret == 0)
		goto no_payload_free;

	ret = keyctl_instantiate_key_common(id, iov, ioc, ret, ringid);
err:
	if (iov != iovstack)
		kfree(iov);
	return ret;

no_payload_free:
	if (iov != iovstack)
		kfree(iov);
no_payload:
	return keyctl_instantiate_key_common(id, NULL, 0, 0, ringid);
}