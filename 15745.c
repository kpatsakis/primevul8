stl_get_hash_for_edge(int M, stl_hash_edge *edge) {
  return ((edge->key[0] / 23 + edge->key[1] / 19 + edge->key[2] / 17
           + edge->key[3] /13  + edge->key[4] / 11 + edge->key[5] / 7 ) % M);
}