static int areDoubleQuotedStringsEnabled(sqlite3 *db, NameContext *pTopNC){
  if( db->init.busy ) return 1;  /* Always support for legacy schemas */
  if( pTopNC->ncFlags & NC_IsDDL ){
    /* Currently parsing a DDL statement */
    if( sqlite3WritableSchema(db) && (db->flags & SQLITE_DqsDML)!=0 ){
      return 1;
    }
    return (db->flags & SQLITE_DqsDDL)!=0;
  }else{
    /* Currently parsing a DML statement */
    return (db->flags & SQLITE_DqsDML)!=0;
  }
}