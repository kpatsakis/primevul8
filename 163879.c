cmsBool OpenOutput(const char* FileName)
{

	OutFile = fopen(FileName, "wb");
	if (OutFile == NULL) {
		FatalError("Cannot create '%s'", FileName);

	}

	Compressor.err                   = jpeg_std_error(&ErrorHandler.pub);
	ErrorHandler.pub.error_exit      = my_error_exit;
	ErrorHandler.pub.output_message  = my_error_exit;

	Compressor.input_components = Compressor.num_components = 4;

	jpeg_create_compress(&Compressor);
	jpeg_stdio_dest(&Compressor, OutFile);
	return TRUE;
}