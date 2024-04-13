UserInfoChars()
{
    /*
     * RFC 3986 section 3.2.1
     *
     *  userinfo      = *( unreserved / pct-encoded / sub-delims / ":" )
     *  unreserved    = ALPHA / DIGIT / "-" / "." / "_" / "~"
     *  pct-encoded   = "%" HEXDIG HEXDIG
     *  sub-delims    = "!" / "$" / "&" / "'" / "(" / ")" / "*" / "+" / "," / ";" / "="
     */
    static const auto userInfoValid = CharacterSet("userinfo", ":-._~%!$&'()*+,;=") +
                                      CharacterSet::ALPHA +
                                      CharacterSet::DIGIT;
    return userInfoValid;
}