void print_1fun(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	char frame[64] = "";
	snprintf(frame, 64, "rw %d rh %d lm %d tm %d", S.raw_width, S.raw_height,
		S.left_margin, S.top_margin);
	fprintf(outfile, "%s=%s=nFms %02d=%s=bps %02d=%s", P1.normalized_make, P1.normalized_model, P1.raw_count,
		MyCoolRawProcessor.unpack_function_name(), C.raw_bps, frame);
	fprintf(outfile, "\n");
}