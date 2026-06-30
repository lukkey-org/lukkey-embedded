# Utility module.

add_library(lukkey_utils STATIC
    cJSON.c
    event_queue.c
    base64.c
    md5.c
    byte_stuffing.c
)

target_include_directories(lukkey_utils
    PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}
)
