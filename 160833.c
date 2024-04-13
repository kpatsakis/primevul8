randomize_mpi (MPI mpi, size_t nbits, int level)
{
  unsigned char *buffer;

  buffer = get_random_bits (nbits, level, mpi_is_secure (mpi));
  mpi_set_buffer (mpi, buffer, (nbits+7)/8, 0);
  xfree (buffer);
}