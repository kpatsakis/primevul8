_isoent_free(struct isoent *isoent)
{
	struct extr_rec *er, *er_next;

	free(isoent->children_sorted);
	free(isoent->identifier);
	er = isoent->extr_rec_list.first;
	while (er != NULL) {
		er_next = er->next;
		free(er);
		er = er_next;
	}
	free(isoent);
}