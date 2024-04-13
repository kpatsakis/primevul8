	static void convertSignedToUnsigned(const void *src, void *dst, size_t count)
	{
		transform<typename signConverter<Format>::signedToUnsigned>(src, dst, count);
	}