int afSyncFile (AFfilehandle handle)
{
	if (!_af_filehandle_ok(handle))
		return -1;

	if (handle->access == _AF_WRITE_ACCESS)
	{
		int	filefmt = handle->fileFormat;
		int	trackno;

		/* Finish writes on all tracks. */
		for (trackno = 0; trackno < handle->trackCount; trackno++)
		{
			_Track	*track = &handle->tracks[trackno];

			if (track->ms->isDirty() && track->ms->setup(handle, track) == AF_FAIL)
				return -1;

			if (track->ms->sync(handle, track) != AF_SUCCEED)
				return -1;
		}

		/* Update file headers. */
		if (_af_units[filefmt].write.update != NULL &&
			_af_units[filefmt].write.update(handle) != AF_SUCCEED)
			return AF_FAIL;
	}
	else if (handle->access == _AF_READ_ACCESS)
	{
		/* Do nothing. */
	}
	else
	{
		_af_error(AF_BAD_ACCMODE, "unrecognized access mode %d",
			handle->access);
		return AF_FAIL;
	}

	return AF_SUCCEED;
}