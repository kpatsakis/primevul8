static bool isTrivialIntMapping(const _AudioFormat &format, FormatCode code)
{
	return intmappings[code] != NULL &&
		format.pcm.slope == intmappings[code]->slope &&
		format.pcm.intercept == intmappings[code]->intercept;
}