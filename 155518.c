pdf_gsave(fz_context *ctx, pdf_run_processor *pr)
{
	if (pr->gtop == pr->gcap-1)
	{
		pr->gstate = fz_resize_array(ctx, pr->gstate, pr->gcap*2, sizeof(pdf_gstate));
		pr->gcap *= 2;
	}

	memcpy(&pr->gstate[pr->gtop + 1], &pr->gstate[pr->gtop], sizeof(pdf_gstate));

	pr->gtop++;
	pdf_keep_gstate(ctx, &pr->gstate[pr->gtop]);
}