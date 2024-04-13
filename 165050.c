void transform(const void *srcData, void *dstData, size_t count)
{
	typedef typename UnaryFunction::argument_type InputType;
	typedef typename UnaryFunction::result_type OutputType;
	const InputType *src = reinterpret_cast<const InputType *>(srcData);
	OutputType *dst = reinterpret_cast<OutputType *>(dstData);
	std::transform(src, src + count, dst, UnaryFunction());
}