int afGetVirtualByteOrder (AFfilehandle handle, int track)
{
	_Track *currentTrack;

	if (!_af_filehandle_ok(handle))
		return -1;

	if ((currentTrack = _af_filehandle_get_track(handle, track)) == NULL)
		return -1;

	return (currentTrack->v.byteOrder);
}