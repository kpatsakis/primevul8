static SQLITE_NOINLINE void sqlite3ExprDeleteNN(sqlite3 *db, Expr *p){
  assert( p!=0 );
  /* Sanity check: Assert that the IntValue is non-negative if it exists */
  assert( !ExprHasProperty(p, EP_IntValue) || p->u.iValue>=0 );

  assert( !ExprHasProperty(p, EP_WinFunc) || p->y.pWin!=0 || db->mallocFailed );
  assert( p->op!=TK_FUNCTION || ExprHasProperty(p, EP_TokenOnly|EP_Reduced)
          || p->y.pWin==0 || ExprHasProperty(p, EP_WinFunc) );
#ifdef SQLITE_DEBUG
  if( ExprHasProperty(p, EP_Leaf) && !ExprHasProperty(p, EP_TokenOnly) ){
    assert( p->pLeft==0 );
    assert( p->pRight==0 );
    assert( p->x.pSelect==0 );
  }
#endif
  if( !ExprHasProperty(p, (EP_TokenOnly|EP_Leaf)) ){
    /* The Expr.x union is never used at the same time as Expr.pRight */
    assert( p->x.pList==0 || p->pRight==0 );
    if( p->pLeft && p->op!=TK_SELECT_COLUMN ) sqlite3ExprDeleteNN(db, p->pLeft);
    if( p->pRight ){
      assert( !ExprHasProperty(p, EP_WinFunc) );
      sqlite3ExprDeleteNN(db, p->pRight);
    }else if( ExprHasProperty(p, EP_xIsSelect) ){
      assert( !ExprHasProperty(p, EP_WinFunc) );
      sqlite3SelectDelete(db, p->x.pSelect);
    }else{
      sqlite3ExprListDelete(db, p->x.pList);
#ifndef SQLITE_OMIT_WINDOWFUNC
      if( ExprHasProperty(p, EP_WinFunc) ){
        sqlite3WindowDelete(db, p->y.pWin);
      }
#endif
    }
  }
  if( ExprHasProperty(p, EP_MemToken) ) sqlite3DbFree(db, p->u.zToken);
  if( !ExprHasProperty(p, EP_Static) ){
    sqlite3DbFreeNN(db, p);
  }
}