ldns_fget_keyword_data(FILE *f, const char *keyword, const char *k_del, char *data,
               const char *d_del, size_t data_limit)
{
       return ldns_fget_keyword_data_l(f, keyword, k_del, data, d_del,
		       data_limit, NULL);
}