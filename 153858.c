void sqlite3ExprListDelete(sqlite3 *db, ExprList *pList){
  if( pList ) exprListDeleteNN(db, pList);
}