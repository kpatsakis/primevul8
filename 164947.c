	static void convertInt(const void *src, void *dst, int count)
	{
		transform<shift<Input, Output> >(src, dst, count);
	}