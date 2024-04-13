stl_load_edge_nearby(stl_file *stl, stl_hash_edge *edge,
                     stl_vertex *a, stl_vertex *b, float tolerance) {
  float diff_x;
  float diff_y;
  float diff_z;
  float max_diff;
  unsigned vertex1[3];
  unsigned vertex2[3];


  diff_x = ABS(a->x - b->x);
  diff_y = ABS(a->y - b->y);
  diff_z = ABS(a->z - b->z);
  max_diff = STL_MAX(diff_x, diff_y);
  max_diff = STL_MAX(diff_z, max_diff);

  vertex1[0] = (unsigned)((a->x - stl->stats.min.x) / tolerance);
  vertex1[1] = (unsigned)((a->y - stl->stats.min.y) / tolerance);
  vertex1[2] = (unsigned)((a->z - stl->stats.min.z) / tolerance);
  vertex2[0] = (unsigned)((b->x - stl->stats.min.x) / tolerance);
  vertex2[1] = (unsigned)((b->y - stl->stats.min.y) / tolerance);
  vertex2[2] = (unsigned)((b->z - stl->stats.min.z) / tolerance);

  if(   (vertex1[0] == vertex2[0])
        && (vertex1[1] == vertex2[1])
        && (vertex1[2] == vertex2[2])) {
    /* Both vertices hash to the same value */
    return 0;
  }

  if(diff_x == max_diff) {
    if(a->x > b->x) {
      memcpy(&edge->key[0], vertex1, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex2, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], vertex2, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex1, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  } else if(diff_y == max_diff) {
    if(a->y > b->y) {
      memcpy(&edge->key[0], vertex1, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex2, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], vertex2, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex1, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  } else {
    if(a->z > b->z) {
      memcpy(&edge->key[0], vertex1, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex2, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], vertex2, sizeof(stl_vertex));
      memcpy(&edge->key[3], vertex1, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  }
  return 1;
}