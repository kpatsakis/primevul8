void _Track::print()
{
	fprintf(stderr, "totalfframes %ld\n", totalfframes);
	fprintf(stderr, "nextfframe %ld\n", nextfframe);
	fprintf(stderr, "frames2ignore %ld\n", frames2ignore);
	fprintf(stderr, "fpos_first_frame %ld\n", fpos_first_frame);
	fprintf(stderr, "fpos_next_frame %ld\n", fpos_next_frame);
	fprintf(stderr, "fpos_after_data %ld\n", fpos_after_data);
	fprintf(stderr, "totalvframes %ld\n", totalvframes);
	fprintf(stderr, "nextvframe %ld\n", nextvframe);
	fprintf(stderr, "data_size %ld\n", data_size);
}