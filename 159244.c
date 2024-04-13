int sqlite3ResolveSelfReference(
  Parse *pParse,   /* Parsing context */
  Table *pTab,     /* The table being referenced, or NULL */
  int type,        /* NC_IsCheck, NC_PartIdx, NC_IdxExpr, NC_GenCol, or 0 */
  Expr *pExpr,     /* Expression to resolve.  May be NULL. */
  ExprList *pList  /* Expression list to resolve.  May be NULL. */
){
  SrcList sSrc;                   /* Fake SrcList for pParse->pNewTable */
  NameContext sNC;                /* Name context for pParse->pNewTable */
  int rc;

  assert( type==0 || pTab!=0 );
  assert( type==NC_IsCheck || type==NC_PartIdx || type==NC_IdxExpr
          || type==NC_GenCol || pTab==0 );
  memset(&sNC, 0, sizeof(sNC));
  memset(&sSrc, 0, sizeof(sSrc));
  if( pTab ){
    sSrc.nSrc = 1;
    sSrc.a[0].zName = pTab->zName;
    sSrc.a[0].pTab = pTab;
    sSrc.a[0].iCursor = -1;
  }
  sNC.pParse = pParse;
  sNC.pSrcList = &sSrc;
  sNC.ncFlags = type | NC_IsDDL;
  if( (rc = sqlite3ResolveExprNames(&sNC, pExpr))!=SQLITE_OK ) return rc;
  if( pList ) rc = sqlite3ResolveExprListNames(&sNC, pList);
  return rc;
}