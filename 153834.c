static Expr *exprDup(sqlite3 *db, Expr *p, int dupFlags, u8 **pzBuffer){
  Expr *pNew;           /* Value to return */
  u8 *zAlloc;           /* Memory space from which to build Expr object */
  u32 staticFlag;       /* EP_Static if space not obtained from malloc */

  assert( db!=0 );
  assert( p );
  assert( dupFlags==0 || dupFlags==EXPRDUP_REDUCE );
  assert( pzBuffer==0 || dupFlags==EXPRDUP_REDUCE );

  /* Figure out where to write the new Expr structure. */
  if( pzBuffer ){
    zAlloc = *pzBuffer;
    staticFlag = EP_Static;
  }else{
    zAlloc = sqlite3DbMallocRawNN(db, dupedExprSize(p, dupFlags));
    staticFlag = 0;
  }
  pNew = (Expr *)zAlloc;

  if( pNew ){
    /* Set nNewSize to the size allocated for the structure pointed to
    ** by pNew. This is either EXPR_FULLSIZE, EXPR_REDUCEDSIZE or
    ** EXPR_TOKENONLYSIZE. nToken is set to the number of bytes consumed
    ** by the copy of the p->u.zToken string (if any).
    */
    const unsigned nStructSize = dupedExprStructSize(p, dupFlags);
    const int nNewSize = nStructSize & 0xfff;
    int nToken;
    if( !ExprHasProperty(p, EP_IntValue) && p->u.zToken ){
      nToken = sqlite3Strlen30(p->u.zToken) + 1;
    }else{
      nToken = 0;
    }
    if( dupFlags ){
      assert( ExprHasProperty(p, EP_Reduced)==0 );
      memcpy(zAlloc, p, nNewSize);
    }else{
      u32 nSize = (u32)exprStructSize(p);
      memcpy(zAlloc, p, nSize);
      if( nSize<EXPR_FULLSIZE ){ 
        memset(&zAlloc[nSize], 0, EXPR_FULLSIZE-nSize);
      }
    }

    /* Set the EP_Reduced, EP_TokenOnly, and EP_Static flags appropriately. */
    pNew->flags &= ~(EP_Reduced|EP_TokenOnly|EP_Static|EP_MemToken);
    pNew->flags |= nStructSize & (EP_Reduced|EP_TokenOnly);
    pNew->flags |= staticFlag;

    /* Copy the p->u.zToken string, if any. */
    if( nToken ){
      char *zToken = pNew->u.zToken = (char*)&zAlloc[nNewSize];
      memcpy(zToken, p->u.zToken, nToken);
    }

    if( 0==((p->flags|pNew->flags) & (EP_TokenOnly|EP_Leaf)) ){
      /* Fill in the pNew->x.pSelect or pNew->x.pList member. */
      if( ExprHasProperty(p, EP_xIsSelect) ){
        pNew->x.pSelect = sqlite3SelectDup(db, p->x.pSelect, dupFlags);
      }else{
        pNew->x.pList = sqlite3ExprListDup(db, p->x.pList, dupFlags);
      }
    }

    /* Fill in pNew->pLeft and pNew->pRight. */
    if( ExprHasProperty(pNew, EP_Reduced|EP_TokenOnly|EP_WinFunc) ){
      zAlloc += dupedExprNodeSize(p, dupFlags);
      if( !ExprHasProperty(pNew, EP_TokenOnly|EP_Leaf) ){
        pNew->pLeft = p->pLeft ?
                      exprDup(db, p->pLeft, EXPRDUP_REDUCE, &zAlloc) : 0;
        pNew->pRight = p->pRight ?
                       exprDup(db, p->pRight, EXPRDUP_REDUCE, &zAlloc) : 0;
      }
#ifndef SQLITE_OMIT_WINDOWFUNC
      if( ExprHasProperty(p, EP_WinFunc) ){
        pNew->y.pWin = sqlite3WindowDup(db, pNew, p->y.pWin);
        assert( ExprHasProperty(pNew, EP_WinFunc) );
      }
#endif /* SQLITE_OMIT_WINDOWFUNC */
      if( pzBuffer ){
        *pzBuffer = zAlloc;
      }
    }else{
      if( !ExprHasProperty(p, EP_TokenOnly|EP_Leaf) ){
        if( pNew->op==TK_SELECT_COLUMN ){
          pNew->pLeft = p->pLeft;
          assert( p->iColumn==0 || p->pRight==0 );
          assert( p->pRight==0  || p->pRight==p->pLeft );
        }else{
          pNew->pLeft = sqlite3ExprDup(db, p->pLeft, 0);
        }
        pNew->pRight = sqlite3ExprDup(db, p->pRight, 0);
      }
    }
  }
  return pNew;
}