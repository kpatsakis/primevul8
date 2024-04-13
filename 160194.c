void print_2fun(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	fprintf(outfile, "// %s %s", P1.make, P1.model);
	if (C.cam_mul[0] > 0)
	{
		fprintf(outfile, "\n'As shot' WB:");
		for (int c = 0; c < 4; c++)
			fprintf(outfile, " %.3f", C.cam_mul[c]);
	}
	if (C.WB_Coeffs[LIBRAW_WBI_Auto][0] > 0)
	{
		fprintf(outfile, "\n'Camera Auto' WB:");
		for (int c = 0; c < 4; c++)
			fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Auto][c]);
	}
	if (C.WB_Coeffs[LIBRAW_WBI_Measured][0] > 0)
	{
		fprintf(outfile, "\n'Camera Measured' WB:");
		for (int c = 0; c < 4; c++)
			fprintf(outfile, " %d", C.WB_Coeffs[LIBRAW_WBI_Measured][c]);
	}
	fprintf(outfile, "\n\n");
}