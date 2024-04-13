BIO* rdg_get_front_bio_and_take_ownership(rdpRdg* rdg)
{
	if (!rdg)
		return NULL;

	rdg->attached = TRUE;
	return rdg->frontBio;
}