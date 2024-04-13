void print_wbfun(FILE* outfile, LibRaw& MyCoolRawProcessor, std::string& fn)
{
	const char *CamMakerName = LibRaw::cameramakeridx2maker(P1.maker_index);
	fprintf(outfile, "// %s %s\n", P1.make, P1.model);
	for (int cnt = 0; cnt < nEXIF_LightSources; cnt++)
		if (C.WB_Coeffs[cnt][0])
		{
			fprintf(outfile, "{LIBRAW_CAMERAMAKER_%s, \"%s\", %d, {%6.5ff, 1.0f, %6.5ff, ", CamMakerName,
				P1.normalized_model, cnt,
				C.WB_Coeffs[cnt][0] / (float)C.WB_Coeffs[cnt][1],
				C.WB_Coeffs[cnt][2] / (float)C.WB_Coeffs[cnt][1]);
			if (C.WB_Coeffs[cnt][1] == C.WB_Coeffs[cnt][3])
				fprintf(outfile, "1.0f}},\n");
			else
				fprintf(outfile, "%6.5ff}},\n",
				C.WB_Coeffs[cnt][3] / (float)C.WB_Coeffs[cnt][1]);
		}
	if (C.WB_Coeffs[LIBRAW_WBI_Sunset][0])
	{
		fprintf(outfile, "{LIBRAW_CAMERAMAKER_%s, \"%s\", %d, {%6.5ff, 1.0f, %6.5ff, ", CamMakerName,
			P1.normalized_model, LIBRAW_WBI_Sunset,
			C.WB_Coeffs[LIBRAW_WBI_Sunset][0] /
			(float)C.WB_Coeffs[LIBRAW_WBI_Sunset][1],
			C.WB_Coeffs[LIBRAW_WBI_Sunset][2] /
			(float)C.WB_Coeffs[LIBRAW_WBI_Sunset][1]);
		if (C.WB_Coeffs[LIBRAW_WBI_Sunset][1] ==
			C.WB_Coeffs[LIBRAW_WBI_Sunset][3])
			fprintf(outfile, "1.0f}},\n");
		else
			fprintf(outfile, "%6.5ff}},\n", C.WB_Coeffs[LIBRAW_WBI_Sunset][3] /
			(float)C.WB_Coeffs[LIBRAW_WBI_Sunset][1]);
	}

	for (int cnt = 0; cnt < 64; cnt++)
		if (C.WBCT_Coeffs[cnt][0])
		{
			fprintf(outfile, "{LIBRAW_CAMERAMAKER_%s, \"%s\", %d, {%6.5ff, 1.0f, %6.5ff, ", CamMakerName,
				P1.normalized_model, (int)C.WBCT_Coeffs[cnt][0],
				C.WBCT_Coeffs[cnt][1] / C.WBCT_Coeffs[cnt][2],
				C.WBCT_Coeffs[cnt][3] / C.WBCT_Coeffs[cnt][2]);
			if (C.WBCT_Coeffs[cnt][2] == C.WBCT_Coeffs[cnt][4])
				fprintf(outfile, "1.0f}},\n");
			else
				fprintf(outfile, "%6.5ff}},\n",
				C.WBCT_Coeffs[cnt][4] / C.WBCT_Coeffs[cnt][2]);
		}
		else
			break;
	fprintf(outfile, "\n");
}