static status _afOpenFile (int access, AFvirtualfile *vf, const char *filename,
	AFfilehandle *file, AFfilesetup filesetup)
{
	int	fileFormat = AF_FILE_UNKNOWN;
	int	implemented = true;
	char	*formatName;
	status	(*initfunc) (AFfilesetup, AFfilehandle);

	int		userSampleFormat = 0;
	double		userSampleRate = 0.0;
	_PCMInfo	userPCM;
	bool		userFormatSet = false;

	int	t;

	AFfilehandle	filehandle = AF_NULL_FILEHANDLE;
	AFfilesetup	completesetup = AF_NULL_FILESETUP;

	*file = AF_NULL_FILEHANDLE;

	if (access == _AF_WRITE_ACCESS || filesetup != AF_NULL_FILESETUP)
	{
		if (!_af_filesetup_ok(filesetup))
			return AF_FAIL;

		fileFormat = filesetup->fileFormat;
		if (access == _AF_READ_ACCESS && fileFormat != AF_FILE_RAWDATA)
		{
			_af_error(AF_BAD_FILESETUP,
				"warning: opening file for read access: "
				"ignoring file setup with non-raw file format");
			filesetup = AF_NULL_FILESETUP;
			fileFormat = _af_identify(vf, &implemented);
		}
	}
	else if (filesetup == AF_NULL_FILESETUP)
		fileFormat = _af_identify(vf, &implemented);

	if (fileFormat == AF_FILE_UNKNOWN)
	{
		if (filename != NULL)
			_af_error(AF_BAD_NOT_IMPLEMENTED,
				"'%s': unrecognized audio file format",
				filename);
		else
			_af_error(AF_BAD_NOT_IMPLEMENTED,
				"unrecognized audio file format");
		return AF_FAIL;
	}

	formatName = _af_units[fileFormat].name;

	if (!implemented)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"%s format not currently supported", formatName);
	}

	assert(_af_units[fileFormat].completesetup != NULL);
	assert(_af_units[fileFormat].read.init != NULL);

	if (access == _AF_WRITE_ACCESS &&
		_af_units[fileFormat].write.init == NULL)
	{
		_af_error(AF_BAD_NOT_IMPLEMENTED,
			"%s format is currently supported for reading only",
			formatName);
		return AF_FAIL;
	}

	completesetup = NULL;
	if (filesetup != AF_NULL_FILESETUP)
	{
		userSampleFormat = filesetup->tracks[0].f.sampleFormat;
		userPCM = filesetup->tracks[0].f.pcm;
		userSampleRate = filesetup->tracks[0].f.sampleRate;
		userFormatSet = true;
		if ((completesetup = _af_units[fileFormat].completesetup(filesetup)) == NULL)
			return AF_FAIL;
	}

	filehandle = (AFfilehandle) _af_malloc(sizeof (_AFfilehandle));
	if (filehandle == NULL)
	{
		if (completesetup)
			afFreeFileSetup(completesetup);
		return AF_FAIL;
	}
	memset(filehandle, 0, sizeof (_AFfilehandle));

	filehandle->valid = _AF_VALID_FILEHANDLE;
	filehandle->fh = vf;
	filehandle->access = access;
	if (filename != NULL)
		filehandle->fileName = strdup(filename);
	else
		filehandle->fileName = NULL;
	filehandle->fileFormat = fileFormat;
	filehandle->formatSpecific = NULL;

	initfunc = (access == _AF_READ_ACCESS) ?
		_af_units[fileFormat].read.init : _af_units[fileFormat].write.init;

	if (initfunc(completesetup, filehandle) != AF_SUCCEED)
	{
		freeFileHandle(filehandle);
		filehandle = AF_NULL_FILEHANDLE;
		if (completesetup)
			afFreeFileSetup(completesetup);
		return AF_FAIL;
	}

	if (completesetup)
	{
		afFreeFileSetup(completesetup);
		completesetup = NULL;
	}

	/*
		Initialize virtual format.
	*/
	for (t=0; t<filehandle->trackCount; t++)
	{
		_Track	*track = &filehandle->tracks[t];

		track->v = track->f;

		if (userFormatSet)
		{
			track->v.sampleFormat = userSampleFormat;
			track->v.pcm = userPCM;
			track->v.sampleRate = userSampleRate;
		}

		track->v.compressionType = AF_COMPRESSION_NONE;
		track->v.compressionParams = NULL;

#if WORDS_BIGENDIAN
		track->v.byteOrder = AF_BYTEORDER_BIGENDIAN;
#else
		track->v.byteOrder = AF_BYTEORDER_LITTLEENDIAN;
#endif

		track->ms = new ModuleState();
		if (track->ms->init(filehandle, track) == AF_FAIL)
		{
			freeFileHandle(filehandle);
			return AF_FAIL;
		}
	}

	*file = filehandle;

	return AF_SUCCEED;
}