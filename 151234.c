void copy_structures(struct activity *act[], unsigned int id_seq[],
		     struct record_header record_hdr[], int dest, int src)
{
	int i, p;

	memcpy(&record_hdr[dest], &record_hdr[src], RECORD_HEADER_SIZE);

	for (i = 0; i < NR_ACT; i++) {

		if (!id_seq[i])
			continue;

		p = get_activity_position(act, id_seq[i], EXIT_IF_NOT_FOUND);

		memcpy(act[p]->buf[dest], act[p]->buf[src],
		       (size_t) act[p]->msize * (size_t) act[p]->nr[src] * (size_t) act[p]->nr2);
		act[p]->nr[dest] = act[p]->nr[src];
	}
}