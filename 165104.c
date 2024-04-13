void afInitFrameCount (AFfilesetup setup, int trackid, AFfileoffset count)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (count < 0)
	{
		_af_error(AF_BAD_FRAMECNT, "invalid frame count %d", count);
		return;
	}

	track->frameCount = count;
	track->frameCountSet = true;
}