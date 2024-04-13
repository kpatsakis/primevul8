int imap_account_match (const ACCOUNT* a1, const ACCOUNT* a2)
{
  IMAP_DATA* a1_idata = imap_conn_find (a1, MUTT_IMAP_CONN_NONEW);
  IMAP_DATA* a2_idata = imap_conn_find (a2, MUTT_IMAP_CONN_NONEW);
  const ACCOUNT* a1_canon = a1_idata == NULL ? a1 : &a1_idata->conn->account;
  const ACCOUNT* a2_canon = a2_idata == NULL ? a2 : &a2_idata->conn->account;

  return mutt_account_match (a1_canon, a2_canon);
}