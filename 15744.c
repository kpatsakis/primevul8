stl_load_edge_exact(stl_file *stl, stl_hash_edge *edge,
                    stl_vertex *a, stl_vertex *b) {

  float diff_x;
  float diff_y;
  float diff_z;
  float max_diff;

  if (stl->error) return;

  diff_x = ABS(a->x - b->x);
  diff_y = ABS(a->y - b->y);
  diff_z = ABS(a->z - b->z);
  max_diff = STL_MAX(diff_x, diff_y);
  max_diff = STL_MAX(diff_z, max_diff);
  stl->stats.shortest_edge = STL_MIN(max_diff, stl->stats.shortest_edge);

  if(diff_x == max_diff) {
    if(a->x > b->x) {
      memcpy(&edge->key[0], a, sizeof(stl_vertex));
      memcpy(&edge->key[3], b, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], b, sizeof(stl_vertex));
      memcpy(&edge->key[3], a, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  } else if(diff_y == max_diff) {
    if(a->y > b->y) {
      memcpy(&edge->key[0], a, sizeof(stl_vertex));
      memcpy(&edge->key[3], b, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], b, sizeof(stl_vertex));
      memcpy(&edge->key[3], a, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  } else {
    if(a->z > b->z) {
      memcpy(&edge->key[0], a, sizeof(stl_vertex));
      memcpy(&edge->key[3], b, sizeof(stl_vertex));
    } else {
      memcpy(&edge->key[0], b, sizeof(stl_vertex));
      memcpy(&edge->key[3], a, sizeof(stl_vertex));
      edge->which_edge += 3; /* this edge is loaded backwards */
    }
  }
}