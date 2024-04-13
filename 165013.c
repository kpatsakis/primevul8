size_t _AudioFormat::bytesPerFrame(bool stretch3to4) const
{
	return _af_format_frame_size_uncompressed(this, stretch3to4);
}