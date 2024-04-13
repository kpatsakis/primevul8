static uint8_t att_ecode_from_error(int err)
{
	/*
	 * If the error fits in a single byte, treat it as an ATT protocol
	 * error as is. Since "0" is not a valid ATT protocol error code, we map
	 * that to UNLIKELY below.
	 */
	if (err > 0 && err < UINT8_MAX)
		return err;

	/*
	 * Since we allow UNIX errnos, map them to appropriate ATT protocol
	 * and "Common Profile and Service" error codes.
	 */
	switch (err) {
	case -ENOENT:
		return BT_ATT_ERROR_INVALID_HANDLE;
	case -ENOMEM:
		return BT_ATT_ERROR_INSUFFICIENT_RESOURCES;
	case -EALREADY:
		return BT_ERROR_ALREADY_IN_PROGRESS;
	case -EOVERFLOW:
		return BT_ERROR_OUT_OF_RANGE;
	}

	return BT_ATT_ERROR_UNLIKELY;
}