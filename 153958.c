int sqlite3WhereExplainOneScan(
  Parse *pParse,                  /* Parse context */
  SrcList *pTabList,              /* Table list this loop refers to */
  WhereLevel *pLevel,             /* Scan to write OP_Explain opcode for */
  u16 wctrlFlags                  /* Flags passed to sqlite3WhereBegin() */
){
  int ret = 0;
#if !defined(SQLITE_DEBUG) && !defined(SQLITE_ENABLE_STMT_SCANSTATUS)
  if( sqlite3ParseToplevel(pParse)->explain==2 )
#endif
  {
    struct SrcList_item *pItem = &pTabList->a[pLevel->iFrom];
    Vdbe *v = pParse->pVdbe;      /* VM being constructed */
    sqlite3 *db = pParse->db;     /* Database handle */
    int isSearch;                 /* True for a SEARCH. False for SCAN. */
    WhereLoop *pLoop;             /* The controlling WhereLoop object */
    u32 flags;                    /* Flags that describe this loop */
    char *zMsg;                   /* Text to add to EQP output */
    StrAccum str;                 /* EQP output string */
    char zBuf[100];               /* Initial space for EQP output string */

    pLoop = pLevel->pWLoop;
    flags = pLoop->wsFlags;
    if( (flags&WHERE_MULTI_OR) || (wctrlFlags&WHERE_OR_SUBCLAUSE) ) return 0;

    isSearch = (flags&(WHERE_BTM_LIMIT|WHERE_TOP_LIMIT))!=0
            || ((flags&WHERE_VIRTUALTABLE)==0 && (pLoop->u.btree.nEq>0))
            || (wctrlFlags&(WHERE_ORDERBY_MIN|WHERE_ORDERBY_MAX));

    sqlite3StrAccumInit(&str, db, zBuf, sizeof(zBuf), SQLITE_MAX_LENGTH);
    sqlite3_str_appendall(&str, isSearch ? "SEARCH" : "SCAN");
    if( pItem->pSelect ){
      sqlite3_str_appendf(&str, " SUBQUERY %u", pItem->pSelect->selId);
    }else{
      sqlite3_str_appendf(&str, " TABLE %s", pItem->zName);
    }

    if( pItem->zAlias ){
      sqlite3_str_appendf(&str, " AS %s", pItem->zAlias);
    }
    if( (flags & (WHERE_IPK|WHERE_VIRTUALTABLE))==0 ){
      const char *zFmt = 0;
      Index *pIdx;

      assert( pLoop->u.btree.pIndex!=0 );
      pIdx = pLoop->u.btree.pIndex;
      assert( !(flags&WHERE_AUTO_INDEX) || (flags&WHERE_IDX_ONLY) );
      if( !HasRowid(pItem->pTab) && IsPrimaryKeyIndex(pIdx) ){
        if( isSearch ){
          zFmt = "PRIMARY KEY";
        }
      }else if( flags & WHERE_PARTIALIDX ){
        zFmt = "AUTOMATIC PARTIAL COVERING INDEX";
      }else if( flags & WHERE_AUTO_INDEX ){
        zFmt = "AUTOMATIC COVERING INDEX";
      }else if( flags & WHERE_IDX_ONLY ){
        zFmt = "COVERING INDEX %s";
      }else{
        zFmt = "INDEX %s";
      }
      if( zFmt ){
        sqlite3_str_append(&str, " USING ", 7);
        sqlite3_str_appendf(&str, zFmt, pIdx->zName);
        explainIndexRange(&str, pLoop);
      }
    }else if( (flags & WHERE_IPK)!=0 && (flags & WHERE_CONSTRAINT)!=0 ){
      const char *zRangeOp;
      if( flags&(WHERE_COLUMN_EQ|WHERE_COLUMN_IN) ){
        zRangeOp = "=";
      }else if( (flags&WHERE_BOTH_LIMIT)==WHERE_BOTH_LIMIT ){
        zRangeOp = ">? AND rowid<";
      }else if( flags&WHERE_BTM_LIMIT ){
        zRangeOp = ">";
      }else{
        assert( flags&WHERE_TOP_LIMIT);
        zRangeOp = "<";
      }
      sqlite3_str_appendf(&str, 
          " USING INTEGER PRIMARY KEY (rowid%s?)",zRangeOp);
    }
#ifndef SQLITE_OMIT_VIRTUALTABLE
    else if( (flags & WHERE_VIRTUALTABLE)!=0 ){
      sqlite3_str_appendf(&str, " VIRTUAL TABLE INDEX %d:%s",
                  pLoop->u.vtab.idxNum, pLoop->u.vtab.idxStr);
    }
#endif
#ifdef SQLITE_EXPLAIN_ESTIMATED_ROWS
    if( pLoop->nOut>=10 ){
      sqlite3_str_appendf(&str, " (~%llu rows)",
             sqlite3LogEstToInt(pLoop->nOut));
    }else{
      sqlite3_str_append(&str, " (~1 row)", 9);
    }
#endif
    zMsg = sqlite3StrAccumFinish(&str);
    sqlite3ExplainBreakpoint("",zMsg);
    ret = sqlite3VdbeAddOp4(v, OP_Explain, sqlite3VdbeCurrentAddr(v),
                            pParse->addrExplain, 0, zMsg,P4_DYNAMIC);
  }
  return ret;
}