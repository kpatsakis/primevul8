void sqlite3PExprAddSelect(Parse *pParse, Expr *pExpr, Select *pSelect){
  if( pExpr ){
    pExpr->x.pSelect = pSelect;
    ExprSetProperty(pExpr, EP_xIsSelect|EP_Subquery);
    sqlite3ExprSetHeightAndFlags(pParse, pExpr);
  }else{
    assert( pParse->db->mallocFailed );
    sqlite3SelectDelete(pParse->db, pSelect);
  }
}