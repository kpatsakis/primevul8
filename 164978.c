void afInitDataOffset (AFfilesetup setup, int trackid, AFfileoffset offset)
{
	_TrackSetup	*track;

	if (!_af_filesetup_ok(setup))
		return;

	if ((track = _af_filesetup_get_tracksetup(setup, trackid)) == NULL)
		return;

	if (offset < 0)
	{
		_af_error(AF_BAD_DATAOFFSET, "invalid data offset %d", offset);
		return;
	}

	track->dataOffset = offset;
	track->dataOffsetSet = true;
}