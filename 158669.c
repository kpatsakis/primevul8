s_tlslog(gstring * g)
{
if (LOGGING(tls_cipher) && tls_in.cipher)
  g = string_append(g, 2, US" X=", tls_in.cipher);
if (LOGGING(tls_certificate_verified) && tls_in.cipher)
  g = string_append(g, 2, US" CV=", tls_in.certificate_verified? "yes":"no");
if (LOGGING(tls_peerdn) && tls_in.peerdn)
  g = string_append(g, 3, US" DN=\"", string_printing(tls_in.peerdn), US"\"");
if (LOGGING(tls_sni) && tls_in.sni)
  g = string_append(g, 3, US" SNI=\"", string_printing(tls_in.sni), US"\"");
return g;
}