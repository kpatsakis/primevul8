int sqlite3IsRowid(const char *z){
  if( sqlite3StrICmp(z, "_ROWID_")==0 ) return 1;
  if( sqlite3StrICmp(z, "ROWID")==0 ) return 1;
  if( sqlite3StrICmp(z, "OID")==0 ) return 1;
  return 0;
}