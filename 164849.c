static bool isTrivialIntClip(const _AudioFormat &format, FormatCode code)
{
	return intmappings[code] != NULL &&
		format.pcm.minClip == intmappings[code]->minClip &&
		format.pcm.maxClip == intmappings[code]->maxClip;
}