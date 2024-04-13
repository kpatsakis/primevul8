void sqlite3ExprDelete(sqlite3 *db, Expr *p){
  if( p ) sqlite3ExprDeleteNN(db, p);
}