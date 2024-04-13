void _af_print_frame (AFframecount frameno, double *frame, int nchannels,
	char *formatstring, int numberwidth,
	double slope, double intercept, double minclip, double maxclip)
{
	char linebuf[81];
	int wavewidth = 78 - numberwidth*nchannels - 6;
	int c;

	memset(linebuf, ' ', 80);
	linebuf[0] = '|';
	linebuf[wavewidth-1] = '|';
	linebuf[wavewidth] = 0;

	printf("%05" AF_FRAMECOUNT_PRINT_FMT " ", frameno);

	for (c=0; c < nchannels; c++)
	{
		double pcm = frame[c];
		printf(formatstring, pcm);
	}
	for (c=0; c < nchannels; c++)
	{
		double pcm = frame[c], volts;
		if (maxclip > minclip)
		{
			if (pcm < minclip) pcm = minclip;
			if (pcm > maxclip) pcm = maxclip;
		}
		volts = (pcm - intercept) / slope;
		linebuf[(int)((volts/2 + 0.5)*(wavewidth-3)) + 1] = '0' + c;
	}
	printf("%s\n", linebuf);
}