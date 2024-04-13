void _af_print_channel_matrix (double *matrix, int fchans, int vchans)
{
	int v, f;

	if (!matrix)
	{
		printf("NULL");
		return;
	}

	printf("{");
	for (v=0; v < vchans; v++)
	{
		if (v) printf(" ");
		printf("{");
		for (f=0; f < fchans; f++)
		{
			if (f) printf(" ");
			printf("%5.2f", *(matrix + v*fchans + f));
		}
		printf("}");
	}
	printf("}");
}