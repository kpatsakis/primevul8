extra_setup_location(struct isoent *isoent, int location)
{
	struct extr_rec *rec;
	int cnt;

	cnt = 0;
	rec = isoent->extr_rec_list.first;
	isoent->extr_rec_list.current = rec;
	while (rec) {
		cnt++;
		rec->location = location++;
		rec->offset = 0;
		rec = rec->next;
	}
	return (cnt);
}