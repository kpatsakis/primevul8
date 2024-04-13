community_direct_str (int direct)
{
  switch (direct)
    {
    case COMMUNITY_DENY:
      return "deny";
    case COMMUNITY_PERMIT:
      return "permit";
    default:
      return "unknown";
    }
}