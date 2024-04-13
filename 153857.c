void sqlite3WhereAddScanStatus(
  Vdbe *v,                        /* Vdbe to add scanstatus entry to */
  SrcList *pSrclist,              /* FROM clause pLvl reads data from */
  WhereLevel *pLvl,               /* Level to add scanstatus() entry for */
  int addrExplain                 /* Address of OP_Explain (or 0) */
){
  const char *zObj = 0;
  WhereLoop *pLoop = pLvl->pWLoop;
  if( (pLoop->wsFlags & WHERE_VIRTUALTABLE)==0  &&  pLoop->u.btree.pIndex!=0 ){
    zObj = pLoop->u.btree.pIndex->zName;
  }else{
    zObj = pSrclist->a[pLvl->iFrom].zName;
  }
  sqlite3VdbeScanStatus(
      v, addrExplain, pLvl->addrBody, pLvl->addrVisit, pLoop->nOut, zObj
  );
}