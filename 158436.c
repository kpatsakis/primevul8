transport_write_reset(int options)
{
if (!(options & topt_continuation)) chunk_ptr = deliver_out_buffer;
nl_partial_match = -1;
nl_check_length = nl_escape_length = 0;
}