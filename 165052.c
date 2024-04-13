status _af_set_sample_format (_AudioFormat *f, int sampleFormat, int sampleWidth)
{
	switch (sampleFormat)
	{
		case AF_SAMPFMT_UNSIGNED:
		case AF_SAMPFMT_TWOSCOMP:
		if (sampleWidth < 1 || sampleWidth > 32)
		{
			_af_error(AF_BAD_SAMPFMT,
				"illegal sample width %d for integer data",
				sampleWidth);
			return AF_FAIL;
		}
		else
		{
			int bytes;

			f->sampleFormat = sampleFormat;
			f->sampleWidth = sampleWidth;

			bytes = _af_format_sample_size_uncompressed(f, false);

			if (sampleFormat == AF_SAMPFMT_TWOSCOMP)
				f->pcm = _af_default_signed_integer_pcm_mappings[bytes];
			else
				f->pcm = _af_default_unsigned_integer_pcm_mappings[bytes];
		}
		break;

		case AF_SAMPFMT_FLOAT:
			f->sampleFormat = sampleFormat;
			f->sampleWidth = 32;
			f->pcm = _af_default_float_pcm_mapping;
			break;
		case AF_SAMPFMT_DOUBLE:
			f->sampleFormat = sampleFormat;
			f->sampleWidth = 64;      /*for convenience */
			f->pcm = _af_default_double_pcm_mapping;
			break;
		default:
			_af_error(AF_BAD_SAMPFMT, "unknown sample format %d",
				sampleFormat);
			return AF_FAIL;
	}

	return AF_SUCCEED;
}