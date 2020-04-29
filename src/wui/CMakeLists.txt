add_library(WUI INTERFACE)

target_sources(WUI INTERFACE
  wui.c
  wui_REST_api.c
  wui_helper_funcs.c
  wui_request_parser.c
  http/altcp_proxyconnect.c
  http/fs.c
  http/http_client.c
  http/httpd.c
  progress_data_wrapper.cpp
  lwip.c
  ethernetif.c
  netif_settings.c
)

target_include_directories(
  WUI
  INTERFACE
    ${CMAKE_CURRENT_LIST_DIR}
    http
    resources
)

target_link_libraries(
  WUI
  INTERFACE LwIP
            jsmn::jsmn
            Marlin
  )
