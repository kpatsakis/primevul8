static int rtreeCheckTable(
  sqlite3 *db,                    /* Database handle to access db through */
  const char *zDb,                /* Name of db ("main", "temp" etc.) */
  const char *zTab,               /* Name of rtree table to check */
  char **pzReport                 /* OUT: sqlite3_malloc'd report text */
){
  RtreeCheck check;               /* Common context for various routines */
  sqlite3_stmt *pStmt = 0;        /* Used to find column count of rtree table */
  int bEnd = 0;                   /* True if transaction should be closed */
  int nAux = 0;                   /* Number of extra columns. */

  /* Initialize the context object */
  memset(&check, 0, sizeof(check));
  check.db = db;
  check.zDb = zDb;
  check.zTab = zTab;

  /* If there is not already an open transaction, open one now. This is
  ** to ensure that the queries run as part of this integrity-check operate
  ** on a consistent snapshot.  */
  if( sqlite3_get_autocommit(db) ){
    check.rc = sqlite3_exec(db, "BEGIN", 0, 0, 0);
    bEnd = 1;
  }

  /* Find the number of auxiliary columns */
  if( check.rc==SQLITE_OK ){
    pStmt = rtreeCheckPrepare(&check, "SELECT * FROM %Q.'%q_rowid'", zDb, zTab);
    if( pStmt ){
      nAux = sqlite3_column_count(pStmt) - 2;
      sqlite3_finalize(pStmt);
    }
    check.rc = SQLITE_OK;
  }

  /* Find number of dimensions in the rtree table. */
  pStmt = rtreeCheckPrepare(&check, "SELECT * FROM %Q.%Q", zDb, zTab);
  if( pStmt ){
    int rc;
    check.nDim = (sqlite3_column_count(pStmt) - 1 - nAux) / 2;
    if( check.nDim<1 ){
      rtreeCheckAppendMsg(&check, "Schema corrupt or not an rtree");
    }else if( SQLITE_ROW==sqlite3_step(pStmt) ){
      check.bInt = (sqlite3_column_type(pStmt, 1)==SQLITE_INTEGER);
    }
    rc = sqlite3_finalize(pStmt);
    if( rc!=SQLITE_CORRUPT ) check.rc = rc;
  }

  /* Do the actual integrity-check */
  if( check.nDim>=1 ){
    if( check.rc==SQLITE_OK ){
      rtreeCheckNode(&check, 0, 0, 1);
    }
    rtreeCheckCount(&check, "_rowid", check.nLeaf);
    rtreeCheckCount(&check, "_parent", check.nNonLeaf);
  }

  /* Finalize SQL statements used by the integrity-check */
  sqlite3_finalize(check.pGetNode);
  sqlite3_finalize(check.aCheckMapping[0]);
  sqlite3_finalize(check.aCheckMapping[1]);

  /* If one was opened, close the transaction */
  if( bEnd ){
    int rc = sqlite3_exec(db, "END", 0, 0, 0);
    if( check.rc==SQLITE_OK ) check.rc = rc;
  }
  *pzReport = check.zReport;
  return check.rc;
}