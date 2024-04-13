int afSetVirtualByteOrder (AFfilehandle handle, int track, int byteorder)
{
	_Track *currentTrack;

	if (!_af_filehandle_ok(handle))
		return -1;

	if (NULL == (currentTrack = _af_filehandle_get_track(handle, track)))
		return AF_FAIL;

	if (byteorder != AF_BYTEORDER_BIGENDIAN &&
		byteorder != AF_BYTEORDER_LITTLEENDIAN)
	{
		_af_error(AF_BAD_BYTEORDER, "invalid byte order %d", byteorder);
		return AF_FAIL;
	}

	currentTrack->v.byteOrder = byteorder;
	currentTrack->ms->setDirty();

	return AF_SUCCEED;
}