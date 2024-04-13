bool _AudioFormat::isUncompressed() const
{
	return compressionType == AF_COMPRESSION_NONE;
}