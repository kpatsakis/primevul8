static int addAggInfoFunc(sqlite3 *db, AggInfo *pInfo){
  int i;
  pInfo->aFunc = sqlite3ArrayAllocate(
       db, 
       pInfo->aFunc,
       sizeof(pInfo->aFunc[0]),
       &pInfo->nFunc,
       &i
  );
  return i;
}    