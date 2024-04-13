void _af_setup_free_loops (AFfilesetup setup, int instno)
{
	if (setup->instruments[instno].loops)
	{
		free(setup->instruments[instno].loops);
	}

	setup->instruments[instno].loops = NULL;
	setup->instruments[instno].loopCount = 0;
}