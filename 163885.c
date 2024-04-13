cmsBool Done(void)
{
	jpeg_destroy_decompress(&Decompressor);
	jpeg_destroy_compress(&Compressor);
	return fclose(InFile) + fclose(OutFile);

}