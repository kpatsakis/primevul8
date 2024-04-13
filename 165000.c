AFfileoffset afGetDataOffset (AFfilehandle file, int trackid)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	return track->fpos_first_frame;
}