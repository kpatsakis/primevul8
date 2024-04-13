T hostToLittle(T value)
{
	return _AF_BYTEORDER_NATIVE == AF_BYTEORDER_LITTLEENDIAN ? value : byteswap(value);
}