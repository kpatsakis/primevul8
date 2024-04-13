bool _af_ima_adpcm_format_ok (_AudioFormat *f)
{
	if (f->channelCount != 1)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires 1 channel");
		return false;
	}

	if (f->sampleFormat != AF_SAMPFMT_TWOSCOMP || f->sampleWidth != 16)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires 16-bit signed integer format");
		f->sampleFormat = AF_SAMPFMT_TWOSCOMP;
		f->sampleWidth = 16;
		/* non-fatal */
	}

	if (f->byteOrder != _AF_BYTEORDER_NATIVE)
	{
		_af_error(AF_BAD_COMPRESSION,
			"IMA ADPCM compression requires native byte order");
		f->byteOrder = _AF_BYTEORDER_NATIVE;
		/* non-fatal */
	}

	return true;
}