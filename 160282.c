void LibRaw::dht_interpolate()
{
  DHT dht(*this);
  dht.hide_hots();
  dht.make_hv_dirs();
  //	dht.illustrate_dirs();
  dht.make_greens();
  dht.make_diag_dirs();
  //	dht.illustrate_dirs();
  dht.make_rb();
  dht.restore_hots();
  dht.copy_to_image();
}