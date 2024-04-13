print_anchor(FILE* f, int anchor)
{
  int q = 0;

  fprintf(f, "[");

  if (anchor & ANCHOR_BEGIN_BUF) {
    fprintf(f, "begin-buf");
    q = 1;
  }
  if (anchor & ANCHOR_BEGIN_LINE) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "begin-line");
  }
  if (anchor & ANCHOR_BEGIN_POSITION) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "begin-pos");
  }
  if (anchor & ANCHOR_END_BUF) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "end-buf");
  }
  if (anchor & ANCHOR_SEMI_END_BUF) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "semi-end-buf");
  }
  if (anchor & ANCHOR_END_LINE) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "end-line");
  }
  if (anchor & ANCHOR_ANYCHAR_STAR) {
    if (q) fprintf(f, ", ");
    q = 1;
    fprintf(f, "anychar-star");
  }
  if (anchor & ANCHOR_ANYCHAR_STAR_ML) {
    if (q) fprintf(f, ", ");
    fprintf(f, "anychar-star-pl");
  }

  fprintf(f, "]");
}