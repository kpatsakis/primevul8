void _af_setup_free_instruments (AFfilesetup setup)
{
	int i;

	if (setup->instruments)
	{
		for (i = 0; i < setup->instrumentCount; i++)
			_af_setup_free_loops(setup, i);

		free(setup->instruments);
	}

	setup->instruments = NULL;
	setup->instrumentCount = 0;
}