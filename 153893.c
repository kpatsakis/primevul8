void sqlite3SubselectError(Parse *pParse, int nActual, int nExpect){
  const char *zFmt = "sub-select returns %d columns - expected %d";
  sqlite3ErrorMsg(pParse, zFmt, nActual, nExpect);
}