void afFreeFileSetup (AFfilesetup setup)
{
	if (!_af_filesetup_ok(setup))
		return;

	_af_setup_free_tracks(setup);

	_af_setup_free_instruments(setup);

	if (setup->miscellaneousCount)
	{
		free(setup->miscellaneous);
		setup->miscellaneous = NULL;
		setup->miscellaneousCount = 0;
	}

	memset(setup, 0, sizeof (_AFfilesetup));
	free(setup);
}