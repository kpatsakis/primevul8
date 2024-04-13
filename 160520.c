void print_unpackfun(FILE* outfile, LibRaw& MyCoolRawProcessor, int print_frame, std::string& fn)
{
	char frame[48] = "";
	if (print_frame)
	{
		ushort right_margin = S.raw_width - S.width - S.left_margin;
		ushort bottom_margin = S.raw_height - S.height - S.top_margin;
		snprintf(frame, 48, "F=%dx%dx%dx%d RS=%dx%d", S.left_margin,
			S.top_margin, right_margin, bottom_margin, S.raw_width,
			S.raw_height);
	}
	fprintf(outfile, "%s\t%s\t%s\t%s/%s\n", fn.c_str(),MyCoolRawProcessor.unpack_function_name(), frame, P1.make,	P1.model);
}