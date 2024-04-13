int _af_identify (AFvirtualfile *vf, int *implemented)
{
	AFfileoffset	curpos;
	int		i;

	curpos = af_ftell(vf);

	for (i=0; i<_AF_NUM_UNITS; i++)
	{
		if (_af_units[i].read.recognize &&
			_af_units[i].read.recognize(vf))
		{
			if (implemented != NULL)
				*implemented = _af_units[i].implemented;
			af_fseek(vf, curpos, SEEK_SET);
			return _af_units[i].fileFormat;
		}
	}

	af_fseek(vf, curpos, SEEK_SET);

	if (implemented != NULL)
		*implemented = false;

	return AF_FILE_UNKNOWN;
}