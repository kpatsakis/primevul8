AFfilesetup afNewFileSetup (void)
{
	AFfilesetup	setup;

	setup = _af_malloc(sizeof (_AFfilesetup));
	if (setup == NULL) return AF_NULL_FILESETUP;

	*setup = _af_default_file_setup;

	setup->tracks = _af_tracksetup_new(setup->trackCount);

	setup->instruments = _af_instsetup_new(setup->instrumentCount);

	if (setup->miscellaneousCount == 0)
		setup->miscellaneous = NULL;
	else
	{
		int	i;

		setup->miscellaneous = _af_calloc(setup->miscellaneousCount,
			sizeof (_MiscellaneousSetup));
		for (i=0; i<setup->miscellaneousCount; i++)
		{
			setup->miscellaneous[i].id = i+1;
			setup->miscellaneous[i].type = 0;
			setup->miscellaneous[i].size = 0;
		}
	}

	return setup;
}