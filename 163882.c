cmsBool OpenInput(const char* FileName)
{
	int m;

	lIsITUFax = FALSE;
	InFile  = fopen(FileName, "rb");
	if (InFile == NULL) {
		FatalError("Cannot open '%s'", FileName);
	}

	// Now we can initialize the JPEG decompression object.
	Decompressor.err                 = jpeg_std_error(&ErrorHandler.pub);
	ErrorHandler.pub.error_exit      = my_error_exit;
	ErrorHandler.pub.output_message  = my_error_exit;

	jpeg_create_decompress(&Decompressor);
	jpeg_stdio_src(&Decompressor, InFile);

	for (m = 0; m < 16; m++)
		jpeg_save_markers(&Decompressor, JPEG_APP0 + m, 0xFFFF);

	// setup_read_icc_profile(&Decompressor);

	fseek(InFile, 0, SEEK_SET);
	jpeg_read_header(&Decompressor, TRUE);

	return TRUE;
}