void nfs42_ssc_unregister_ops(void)
{
	nfs42_ssc_unregister(&nfs4_ssc_clnt_ops_tbl);
}