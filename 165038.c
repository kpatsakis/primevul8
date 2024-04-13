	static void convertUnsignedToSigned(const void *src, void *dst, size_t count)
	{
		transform<typename signConverter<Format>::unsignedToSigned>(src, dst, count);
	}