status ModuleState::arrange(AFfilehandle file, _Track *track)
{
	bool isReading = file->access == _AF_READ_ACCESS;
	_AudioFormat in, out;
	FormatCode infc, outfc;
	if (isReading)
	{
		in = track->f;
		out = track->v;
	}
	else
	{
		in = track->v;
		out = track->f;
	}

	infc = getFormatCode(in);
	outfc = getFormatCode(out);

	m_chunks.clear();
	m_chunks.push_back(new Chunk());
	m_chunks.back()->f = in;

	m_modules.clear();

	if (isReading)
		addModule(m_fileModule.get());

	// Convert to native byte order.
	if (in.byteOrder != _AF_BYTEORDER_NATIVE)
	{
		size_t bytesPerSample = in.bytesPerSample(!isReading);
		if (bytesPerSample > 1 && in.compressionType == AF_COMPRESSION_NONE)
			addModule(new SwapModule());
		else
			in.byteOrder = _AF_BYTEORDER_NATIVE;
	}

	// Handle 24-bit integer input format.
	if (in.isInteger() && in.bytesPerSample(false) == 3)
	{
		if (isReading || in.compressionType != AF_COMPRESSION_NONE)
			addModule(new Expand3To4Module(in.isSigned()));
	}

	// Make data signed.
	if (in.isUnsigned())
		addModule(new ConvertSign(infc, false));

	in.pcm = m_chunks.back()->f.pcm;

	// Reverse the unsigned shift for output.
	if (out.isUnsigned())
	{
		const double shift = intmappings[outfc]->minClip;
		out.pcm.intercept += shift;
		out.pcm.minClip += shift;
		out.pcm.maxClip += shift;
	}

	// Clip input samples if necessary.
	if (in.pcm.minClip < in.pcm.maxClip && !isTrivialIntClip(in, infc))
		addModule(new Clip(infc, in.pcm));

	bool alreadyClippedOutput = false;
	bool alreadyTransformedOutput = false;
	// Perform range transformation if input and output PCM mappings differ.
	bool transforming = (in.pcm.slope != out.pcm.slope ||
		in.pcm.intercept != out.pcm.intercept) &&
		!(isTrivialIntMapping(in, infc) &&
		isTrivialIntMapping(out, outfc));

	// Range transformation requires input to be floating-point.
	if (isInteger(infc) && transforming)
	{
		if (infc == kInt32 || outfc == kDouble || outfc == kInt32)
		{
			addConvertIntToFloat(infc, kDouble);
			infc = kDouble;
		}
		else
		{
			addConvertIntToFloat(infc, kFloat);
			infc = kFloat;
		}
	}

	if (transforming && infc == kDouble && isFloat(outfc))
		addModule(new Transform(infc, in.pcm, out.pcm));

	// Add format conversion if needed.
	if (isInteger(infc) && isInteger(outfc))
		addConvertIntToInt(infc, outfc);
	else if (isInteger(infc) && isFloat(outfc))
		addConvertIntToFloat(infc, outfc);
	else if (isFloat(infc) && isInteger(outfc))
	{
		addConvertFloatToInt(infc, outfc, in.pcm, out.pcm);
		alreadyClippedOutput = true;
		alreadyTransformedOutput = true;
	}
	else if (isFloat(infc) && isFloat(outfc))
		addConvertFloatToFloat(infc, outfc);

	if (transforming && !alreadyTransformedOutput && infc != kDouble)
		addModule(new Transform(outfc, in.pcm, out.pcm));

	if (in.channelCount != out.channelCount)
		addModule(new ApplyChannelMatrix(infc, isReading,
			in.channelCount, out.channelCount,
			in.pcm.minClip, in.pcm.maxClip,
			track->channelMatrix));

	// Perform clipping if necessary.
	if (!alreadyClippedOutput)
	{
		if (out.pcm.minClip < out.pcm.maxClip && !isTrivialIntClip(out, outfc))
			addModule(new Clip(outfc, out.pcm));
	}

	// Make data unsigned if necessary.
	if (out.isUnsigned())
		addModule(new ConvertSign(outfc, true));

	// Handle 24-bit integer output format.
	if (out.isInteger() && out.bytesPerSample(false) == 3)
	{
		if (!isReading || out.compressionType != AF_COMPRESSION_NONE)
			addModule(new Compress4To3Module(out.isSigned()));
	}

	if (out.byteOrder != _AF_BYTEORDER_NATIVE)
	{
		size_t bytesPerSample = out.bytesPerSample(isReading);
		if (bytesPerSample > 1 && out.compressionType == AF_COMPRESSION_NONE)
			addModule(new SwapModule());
		else
			out.byteOrder = _AF_BYTEORDER_NATIVE;
	}

	if (!isReading)
		addModule(m_fileModule.get());

	return AF_SUCCEED;
}