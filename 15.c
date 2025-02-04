CellularNetwork::CellularNetwork(const CellularNetwork& network)
    : WirelessNetwork(network) {
  activation_state_ = network.activation_state();
  network_technology_ = network.network_technology();
  roaming_state_ = network.roaming_state();
  restricted_pool_ = network.restricted_pool();
  service_name_ = network.service_name();
  operator_name_ = network.operator_name();
  operator_code_ = network.operator_code();
  payment_url_ = network.payment_url();
  meid_ = network.meid();
  imei_ = network.imei();
  imsi_ = network.imsi();
  esn_ = network.esn();
  mdn_ = network.mdn();
  min_ = network.min();
  model_id_ = network.model_id();
  manufacturer_ = network.manufacturer();
  firmware_revision_ = network.firmware_revision();
  hardware_revision_ = network.hardware_revision();
  last_update_ = network.last_update();
  prl_version_ = network.prl_version();
  type_ = TYPE_CELLULAR;
}
