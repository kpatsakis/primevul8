AFframecount afSeekFrame (AFfilehandle file, int trackid, AFframecount frame)
{
	_Track	*track;

	if (!_af_filehandle_ok(file))
		return -1;

	if (!_af_filehandle_can_read(file))
		return -1;

	if ((track = _af_filehandle_get_track(file, trackid)) == NULL)
		return -1;

	if (track->ms->isDirty() && track->ms->setup(file, track) == AF_FAIL)
		return -1;

	if (frame < 0)
		return track->nextvframe;

	/* Optimize the case of seeking to the current position. */
	if (frame == track->nextvframe)
		return track->nextvframe;

	/* Limit request to the number of frames in the file. */
	if (track->totalvframes != -1)
		if (frame > track->totalvframes)
			frame = track->totalvframes - 1;

	/*
		Now that the modules are not dirty and frame
		represents a valid virtual frame, we call
		_AFsetupmodules again after setting track->nextvframe.

		_AFsetupmodules will look at track->nextvframe and
		compute track->nextfframe in clever and mysterious
		ways.
	*/
	track->nextvframe = frame;

	if (track->ms->setup(file, track) == AF_FAIL)
		return -1;

	return track->nextvframe;
}