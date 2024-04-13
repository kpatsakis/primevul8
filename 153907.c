CollSeq *sqlite3ExprCollSeq(Parse *pParse, Expr *pExpr){
  sqlite3 *db = pParse->db;
  CollSeq *pColl = 0;
  Expr *p = pExpr;
  while( p ){
    int op = p->op;
    if( op==TK_REGISTER ) op = p->op2;
    if( (op==TK_AGG_COLUMN || op==TK_COLUMN || op==TK_TRIGGER)
     && p->y.pTab!=0
    ){
      /* op==TK_REGISTER && p->y.pTab!=0 happens when pExpr was originally
      ** a TK_COLUMN but was previously evaluated and cached in a register */
      int j = p->iColumn;
      if( j>=0 ){
        const char *zColl = p->y.pTab->aCol[j].zColl;
        pColl = sqlite3FindCollSeq(db, ENC(db), zColl, 0);
      }
      break;
    }
    if( op==TK_CAST || op==TK_UPLUS ){
      p = p->pLeft;
      continue;
    }
    if( op==TK_VECTOR ){
      p = p->x.pList->a[0].pExpr;
      continue;
    }
    if( op==TK_COLLATE ){
      pColl = sqlite3GetCollSeq(pParse, ENC(db), 0, p->u.zToken);
      break;
    }
    if( p->flags & EP_Collate ){
      if( p->pLeft && (p->pLeft->flags & EP_Collate)!=0 ){
        p = p->pLeft;
      }else{
        Expr *pNext  = p->pRight;
        /* The Expr.x union is never used at the same time as Expr.pRight */
        assert( p->x.pList==0 || p->pRight==0 );
        if( p->x.pList!=0 
         && !db->mallocFailed
         && ALWAYS(!ExprHasProperty(p, EP_xIsSelect))
        ){
          int i;
          for(i=0; i<p->x.pList->nExpr; i++){
            if( ExprHasProperty(p->x.pList->a[i].pExpr, EP_Collate) ){
              pNext = p->x.pList->a[i].pExpr;
              break;
            }
          }
        }
        p = pNext;
      }
    }else{
      break;
    }
  }
  if( sqlite3CheckCollSeq(pParse, pColl) ){ 
    pColl = 0;
  }
  return pColl;
}