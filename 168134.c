get_nth_protocol_set_vote(int n, const networkstatus_t *vote)
{
  switch (n) {
    case 0: return vote->recommended_client_protocols;
    case 1: return vote->recommended_relay_protocols;
    case 2: return vote->required_client_protocols;
    case 3: return vote->required_relay_protocols;
    default:
      tor_assert_unreached();
      return NULL;
  }
}