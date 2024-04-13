	static void run(const void *src, void *dst, int count)
	{
		transform<intToFloat<Arg, Result> >(src, dst, count);
	}