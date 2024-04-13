long keyctl_instantiate_key(key_serial_t id,
			    const void __user *_payload,
			    size_t plen,
			    key_serial_t ringid)
{
	if (_payload && plen) {
		struct iovec iov[1] = {
			[0].iov_base = (void __user *)_payload,
			[0].iov_len  = plen
		};

		return keyctl_instantiate_key_common(id, iov, 1, plen, ringid);
	}

	return keyctl_instantiate_key_common(id, NULL, 0, 0, ringid);
}