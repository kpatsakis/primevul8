void _af_print_filehandle (AFfilehandle filehandle)
{
	printf("file handle: 0x%p\n", filehandle);

	if (filehandle->valid == _AF_VALID_FILEHANDLE)
		printf("valid\n");
	else
		printf("invalid!\n");

	printf(" access: ");
	if (filehandle->access == _AF_READ_ACCESS)
		putchar('r');
	else
		putchar('w');

	printf(" fileFormat: %d\n", filehandle->fileFormat);

	printf(" instrument count: %d\n", filehandle->instrumentCount);
	printf(" instruments: 0x%p\n", filehandle->instruments);

	printf(" miscellaneous count: %d\n", filehandle->miscellaneousCount);
	printf(" miscellaneous: 0x%p\n", filehandle->miscellaneous);

	printf(" trackCount: %d\n", filehandle->trackCount);
	printf(" tracks: 0x%p\n", filehandle->tracks);
	_af_print_tracks(filehandle);
}