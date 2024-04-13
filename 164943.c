void afInitChannels (AFfilesetup setup, int trackid, int channels)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (channels < 1)
	{
		_af_error(AF_BAD_CHANNELS, "invalid number of channels %d",
			channels);
		return;
	}

	track->f.channelCount = channels;
	track->channelCountSet = true;
}