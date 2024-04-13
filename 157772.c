cell_command_is_var_length(uint8_t command, int linkproto)
{
  /* If linkproto is v2 (2), CELL_VERSIONS is the only variable-length cells
   * work as implemented here. If it's 1, there are no variable-length cells.
   * Tor does not support other versions right now, and so can't negotiate
   * them.
   */
  switch (linkproto) {
  case 1:
    /* Link protocol version 1 has no variable-length cells. */
    return 0;
  case 2:
    /* In link protocol version 2, VERSIONS is the only variable-length cell */
    return command == CELL_VERSIONS;
  case 0:
  case 3:
  default:
    /* In link protocol version 3 and later, and in version "unknown",
     * commands 128 and higher indicate variable-length. VERSIONS is
     * grandfathered in. */
    return command == CELL_VERSIONS || command >= 128;
  }
}