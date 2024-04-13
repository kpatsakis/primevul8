int _af_format_sample_size_uncompressed (const _AudioFormat *format, bool stretch3to4)
{
	int	size = 0;

	switch (format->sampleFormat)
	{
		case AF_SAMPFMT_FLOAT:
			size = sizeof (float);
			break;
		case AF_SAMPFMT_DOUBLE:
			size = sizeof (double);
			break;
		default:
			size = (int) (format->sampleWidth + 7) / 8;
			if (format->compressionType == AF_COMPRESSION_NONE &&
				size == 3 && stretch3to4)
				size = 4;
			break;
	}

	return size;
}