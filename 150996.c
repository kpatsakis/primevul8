static Expr *substExpr(
  SubstContext *pSubst,  /* Description of the substitution */
  Expr *pExpr            /* Expr in which substitution occurs */
){
  if( pExpr==0 ) return 0;
  if( ExprHasProperty(pExpr, EP_FromJoin)
   && pExpr->iRightJoinTable==pSubst->iTable
  ){
    pExpr->iRightJoinTable = pSubst->iNewTable;
  }
  if( pExpr->op==TK_COLUMN && pExpr->iTable==pSubst->iTable ){
    if( pExpr->iColumn<0 ){
      pExpr->op = TK_NULL;
    }else{
      Expr *pNew;
      Expr *pCopy = pSubst->pEList->a[pExpr->iColumn].pExpr;
      Expr ifNullRow;
      assert( pSubst->pEList!=0 && pExpr->iColumn<pSubst->pEList->nExpr );
      assert( pExpr->pLeft==0 && pExpr->pRight==0 );
      if( sqlite3ExprIsVector(pCopy) ){
        sqlite3VectorErrorMsg(pSubst->pParse, pCopy);
      }else{
        sqlite3 *db = pSubst->pParse->db;
        if( pSubst->isLeftJoin && pCopy->op!=TK_COLUMN ){
          memset(&ifNullRow, 0, sizeof(ifNullRow));
          ifNullRow.op = TK_IF_NULL_ROW;
          ifNullRow.pLeft = pCopy;
          ifNullRow.iTable = pSubst->iNewTable;
          pCopy = &ifNullRow;
        }
        pNew = sqlite3ExprDup(db, pCopy, 0);
        if( pNew && pSubst->isLeftJoin ){
          ExprSetProperty(pNew, EP_CanBeNull);
        }
        if( pNew && ExprHasProperty(pExpr,EP_FromJoin) ){
          pNew->iRightJoinTable = pExpr->iRightJoinTable;
          ExprSetProperty(pNew, EP_FromJoin);
        }
        sqlite3ExprDelete(db, pExpr);
        pExpr = pNew;
      }
    }
  }else{
    if( pExpr->op==TK_IF_NULL_ROW && pExpr->iTable==pSubst->iTable ){
      pExpr->iTable = pSubst->iNewTable;
    }
    pExpr->pLeft = substExpr(pSubst, pExpr->pLeft);
    pExpr->pRight = substExpr(pSubst, pExpr->pRight);
    if( ExprHasProperty(pExpr, EP_xIsSelect) ){
      substSelect(pSubst, pExpr->x.pSelect, 1);
    }else{
      substExprList(pSubst, pExpr->x.pList);
    }
  }
  return pExpr;
}