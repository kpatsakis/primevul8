int LibRaw::p1raw(unsigned row, unsigned col)
{
  return (row < raw_height && col < raw_width) ? RAW(row, col) : 0;
}