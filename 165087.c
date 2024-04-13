_InstrumentSetup *_af_instsetup_new (int instrumentCount)
{
	int	i;

	_InstrumentSetup	*instruments;

	if (instrumentCount == 0)
		return NULL;
	instruments = _af_calloc(instrumentCount, sizeof (_InstrumentSetup));
	if (instruments == NULL)
		return NULL;

	for (i=0; i<instrumentCount; i++)
	{
		instruments[i] = _af_default_instrumentsetup;
		instruments[i].id = AF_DEFAULT_INST + i;
		if (instruments[i].loopCount == 0)
			instruments[i].loops = NULL;
		else
		{
			int	j;
			instruments[i].loops = _af_calloc(instruments[i].loopCount, sizeof (_LoopSetup));
			if (instruments[i].loops == NULL)
				return NULL;

			for (j=0; j<instruments[i].loopCount; j++)
				instruments[i].loops[j].id = j+1;
		}
	}

	return instruments;
}