void afInitSampleFormat (AFfilesetup setup, int trackid, int sampfmt, int sampwidth)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	_af_set_sample_format(&track->f, sampfmt, sampwidth);

	track->sampleFormatSet = true;
	track->sampleWidthSet = true;
}