int _af_format_frame_size_uncompressed (const _AudioFormat *fmt, bool stretch3to4)
{
	return _af_format_sample_size_uncompressed(fmt, stretch3to4) *
		fmt->channelCount;
}