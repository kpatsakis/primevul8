static int rdg_bio_new(BIO* bio)
{
	BIO_set_init(bio, 1);
	BIO_set_flags(bio, BIO_FLAGS_SHOULD_RETRY);
	return 1;
}