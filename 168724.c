static BIO_METHOD* BIO_s_rdg(void)
{
	static BIO_METHOD* bio_methods = NULL;

	if (bio_methods == NULL)
	{
		if (!(bio_methods = BIO_meth_new(BIO_TYPE_TSG, "RDGateway")))
			return NULL;

		BIO_meth_set_write(bio_methods, rdg_bio_write);
		BIO_meth_set_read(bio_methods, rdg_bio_read);
		BIO_meth_set_puts(bio_methods, rdg_bio_puts);
		BIO_meth_set_gets(bio_methods, rdg_bio_gets);
		BIO_meth_set_ctrl(bio_methods, rdg_bio_ctrl);
		BIO_meth_set_create(bio_methods, rdg_bio_new);
		BIO_meth_set_destroy(bio_methods, rdg_bio_free);
	}

	return bio_methods;
}