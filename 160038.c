int main(int ac, char *av[])
{
	if (ac < 4)
	{
		usage(av[0]);
		exit(1);
	}
	int colstart = atoi(av[2]);
	int rowstart = atoi(av[3]);
	int channel = 0;
	if (ac > 4) channel = atoi(av[4]);
	int width = 16;
	if (ac > 5) width = atoi(av[5]);
	int height = 4;
	if (ac > 6) height = atoi(av[6]);
	if (width <1 || height<1)
	{
		usage(av[0]);
		exit(1);
	}

	LibRaw_bl lr;

	if (lr.open_file(av[1]) != LIBRAW_SUCCESS)
	{
		fprintf(stderr, "Unable to open file %s\n", av[1]);
		exit(1);
	}
	if ((lr.imgdata.idata.colors == 1 && channel>0) || (channel >3))
	{
		fprintf(stderr, "Incorrect CHANNEL specified: %d\n", channel);
		exit(1);
	}
	if (lr.unpack() != LIBRAW_SUCCESS)
	{
		fprintf(stderr, "Unable to unpack raw data from %s\n", av[1]);
		exit(1);
	}
	lr.adjust_blacklevel();
	printf("%s\t%d-%d-%dx%d\tchannel: %d\n", av[1], colstart, rowstart, width, height, channel);

	printf("%6s", "R\\C");
	for (int col = colstart; col < colstart + width && col < lr.imgdata.sizes.raw_width; col++)
		printf("%6u", col);
	printf("\n");

	if (lr.imgdata.rawdata.raw_image)
	{
		for (int row = rowstart; row < rowstart + height && row < lr.imgdata.sizes.raw_height; row++)
		{
			unsigned rcolors[48];
			if (lr.imgdata.idata.colors > 1)
				for (int c = 0; c < 48; c++)
					rcolors[c] = lr.COLOR(row, c);
			else
				memset(rcolors, 0, sizeof(rcolors));
			unsigned short *rowdata = &lr.imgdata.rawdata.raw_image[row * lr.imgdata.sizes.raw_pitch / 2];
			printf("%6u", row);
			for (int col = colstart; col < colstart + width && col < lr.imgdata.sizes.raw_width; col++)
				if (rcolors[col % 48] == channel) printf("%6u", subtract_bl(rowdata[col],lr.imgdata.color.cblack[channel]));
				else printf("     -");
			printf("\n");
		}
	}
	else if (lr.imgdata.rawdata.color4_image && channel < 4)
	{
		for (int row = rowstart; row < rowstart + height && row < lr.imgdata.sizes.raw_height; row++)
		{
			unsigned short(*rowdata)[4] = &lr.imgdata.rawdata.color4_image[row * lr.imgdata.sizes.raw_pitch / 8];
			printf("%6u", row);
			for (int col = colstart; col < colstart + width && col < lr.imgdata.sizes.raw_width; col++)
				printf("%6u", subtract_bl(rowdata[col][channel],lr.imgdata.color.cblack[channel]));
			printf("\n");
		}
	}
	else if (lr.imgdata.rawdata.color3_image && channel < 3)
	{
		for (int row = rowstart; row < rowstart + height && row < lr.imgdata.sizes.raw_height; row++)
		{
			unsigned short(*rowdata)[3] = &lr.imgdata.rawdata.color3_image[row * lr.imgdata.sizes.raw_pitch / 6];
			printf("%6u", row);
			for (int col = colstart; col < colstart + width && col < lr.imgdata.sizes.raw_width; col++)
				printf("%6u", subtract_bl(rowdata[col][channel],lr.imgdata.color.cblack[channel]));
			printf("\n");
		}
	}
	else
		printf("Unsupported file data (e.g. floating point format), or incorrect channel specified\n");
}