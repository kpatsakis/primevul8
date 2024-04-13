static int resolveOrderByTermToExprList(
  Parse *pParse,     /* Parsing context for error messages */
  Select *pSelect,   /* The SELECT statement with the ORDER BY clause */
  Expr *pE           /* The specific ORDER BY term */
){
  int i;             /* Loop counter */
  ExprList *pEList;  /* The columns of the result set */
  NameContext nc;    /* Name context for resolving pE */
  sqlite3 *db;       /* Database connection */
  int rc;            /* Return code from subprocedures */
  u8 savedSuppErr;   /* Saved value of db->suppressErr */

  assert( sqlite3ExprIsInteger(pE, &i)==0 );
  pEList = pSelect->pEList;

  /* Resolve all names in the ORDER BY term expression
  */
  memset(&nc, 0, sizeof(nc));
  nc.pParse = pParse;
  nc.pSrcList = pSelect->pSrc;
  nc.uNC.pEList = pEList;
  nc.ncFlags = NC_AllowAgg|NC_UEList;
  nc.nErr = 0;
  db = pParse->db;
  savedSuppErr = db->suppressErr;
  db->suppressErr = 1;
  rc = sqlite3ResolveExprNames(&nc, pE);
  db->suppressErr = savedSuppErr;
  if( rc ) return 0;

  /* Try to match the ORDER BY expression against an expression
  ** in the result set.  Return an 1-based index of the matching
  ** result-set entry.
  */
  for(i=0; i<pEList->nExpr; i++){
    if( sqlite3ExprCompare(0, pEList->a[i].pExpr, pE, -1)<2 ){
      return i+1;
    }
  }

  /* If no match, return 0. */
  return 0;
}