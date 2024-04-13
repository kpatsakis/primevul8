CellularNetwork::CellularNetwork(const ServiceInfo* service)
    : WirelessNetwork(service) {
  service_name_ = SafeString(service->name);
  activation_state_ = service->activation_state;
  network_technology_ = service->network_technology;
  roaming_state_ = service->roaming_state;
  restricted_pool_ = service->restricted_pool;
  if (service->carrier_info) {
    operator_name_ = SafeString(service->carrier_info->operator_name);
    operator_code_ = SafeString(service->carrier_info->operator_code);
    payment_url_ = SafeString(service->carrier_info->payment_url);
  }
  if (service->device_info) {
    meid_ = SafeString(service->device_info->MEID);
    imei_ = SafeString(service->device_info->IMEI);
    imsi_ = SafeString(service->device_info->IMSI);
    esn_ = SafeString(service->device_info->ESN);
    mdn_ = SafeString(service->device_info->MDN);
    min_ = SafeString(service->device_info->MIN);
    model_id_ = SafeString(service->device_info->model_id);
    manufacturer_ = SafeString(service->device_info->manufacturer);
    firmware_revision_ = SafeString(service->device_info->firmware_revision);
    hardware_revision_ = SafeString(service->device_info->hardware_revision);
    last_update_ = SafeString(service->device_info->last_update);
    prl_version_ = service->device_info->PRL_version;
  }
  type_ = TYPE_CELLULAR;
}
