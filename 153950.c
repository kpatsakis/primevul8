int sqlite3SelectExprHeight(Select *p){
  int nHeight = 0;
  heightOfSelect(p, &nHeight);
  return nHeight;
}