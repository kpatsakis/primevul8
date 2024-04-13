T bigToHost(T value)
{
	return _AF_BYTEORDER_NATIVE == AF_BYTEORDER_BIGENDIAN ? value : byteswap(value);
}