d_tlslog(gstring * s, address_item * addr)
{
if (LOGGING(tls_cipher) && addr->cipher)
  s = string_append(s, 2, US" X=", addr->cipher);
if (LOGGING(tls_certificate_verified) && addr->cipher)
  s = string_append(s, 2, US" CV=",
    testflag(addr, af_cert_verified)
    ?
#ifdef SUPPORT_DANE
      testflag(addr, af_dane_verified)
    ? "dane"
    :
#endif
      "yes"
    : "no");
if (LOGGING(tls_peerdn) && addr->peerdn)
  s = string_append(s, 3, US" DN=\"", string_printing(addr->peerdn), US"\"");
return s;
}