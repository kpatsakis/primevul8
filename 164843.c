void _af_print_audioformat (_AudioFormat *fmt)
{
	/* sampleRate, channelCount */
	printf("{ %7.2f Hz %d ch ", fmt->sampleRate, fmt->channelCount);

	/* sampleFormat, sampleWidth */
	switch (fmt->sampleFormat)
	{
		case AF_SAMPFMT_TWOSCOMP:
			printf("%db 2 ", fmt->sampleWidth);
			break;
		case AF_SAMPFMT_UNSIGNED:
			printf("%db u ", fmt->sampleWidth);
			break;
		case AF_SAMPFMT_FLOAT:
			printf("flt ");
			break;
		case AF_SAMPFMT_DOUBLE:
			printf("dbl ");
			break;
		default:
			printf("%dsampfmt? ", fmt->sampleFormat);
	}

	/* pcm */
	printf("(%.30g+-%.30g [%.30g,%.30g]) ",
		fmt->pcm.intercept, fmt->pcm.slope,
		fmt->pcm.minClip, fmt->pcm.maxClip);

	/* byteOrder */
	switch (fmt->byteOrder)
	{
		case AF_BYTEORDER_BIGENDIAN:
			printf("big ");
			break;
		case AF_BYTEORDER_LITTLEENDIAN:
			printf("little ");
			break;
		default:
			printf("%dbyteorder? ", fmt->byteOrder);
			break;
	}

	/* compression */
	{
		int idx = _af_compression_index_from_id(fmt->compressionType);
		if (idx < 0)
		{
			printf("%dcompression?", fmt->compressionType);
		}
		else if (fmt->compressionType == AF_COMPRESSION_NONE)
			printf("pcm");
		else
			printf("%s", _af_compression[idx].label);
	}

	printf(" }");
}