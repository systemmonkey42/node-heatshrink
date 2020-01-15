#include "addon.h"
#include "heatshrink_decoder.h"
#include "heatshrink_encoder.h"
#include <stdio.h>

#define NAPI_CALL(env, call)                                                   \
  do {                                                                         \
    napi_status status = (call);                                               \
    if (status != napi_ok) {                                                   \
      const napi_extended_error_info *error_info = NULL;                       \
      napi_get_last_error_info((env), &error_info);                            \
      bool is_pending;                                                         \
      napi_is_exception_pending((env), &is_pending);                           \
      if (!is_pending) {                                                       \
        const char *message = (error_info->error_message == NULL)              \
                                  ? "empty error message"                      \
                                  : error_info->error_message;                 \
        napi_throw_error((env), NULL, message);                                \
        return NULL;                                                           \
      }                                                                        \
    }                                                                          \
  } while (0)

static void encoder_instance_finalizer(napi_env env, void *data, void *hint) {
  printf("Called encoder finalizer..\n");

  printf("Instance data = %p\n", (char *)data);
  heatshrink_encoder_free((heatshrink_encoder *)data);
}

static void decoder_instance_finalizer(napi_env env, void *data, void *hint) {
  printf("Called decoder finalizer..\n");

  printf("Instance data = %p\n", (char *)data);
  heatshrink_decoder_free((heatshrink_decoder *)data);
}

#define NAPI_THIS                                                              \
  napi_value this;                                                             \
  NAPI_CALL(env, napi_get_cb_info(env, info, 0, NULL, &this, NULL))

#define NAPI_ARGS(n)                                                           \
  size_t argc = (n);                                                           \
  napi_value argv[(n)];                                                        \
  napi_value this;                                                             \
  NAPI_CALL(env, napi_get_cb_info(env, info, &argc, argv, &this, NULL))

static napi_value encoder_constructor(napi_env env, napi_callback_info info) {

  NAPI_ARGS(1);

  napi_valuetype argType;
  NAPI_CALL(env, napi_typeof(env, argv[0], &argType));
  if (argType == napi_object) {

    // windowSize: number;
    // lookaheadSize: number;

    int32_t window_size;
    napi_value winsz;
    NAPI_CALL(env, napi_get_named_property(env, argv[0], "windowSize", &winsz));
    NAPI_CALL(env, napi_get_value_int32(env, winsz, &window_size));

    int32_t look_ahead;
    napi_value lahead;
    NAPI_CALL(env,
              napi_get_named_property(env, argv[0], "lookaheadSize", &lahead));
    NAPI_CALL(env, napi_get_value_int32(env, lahead, &look_ahead));

    heatshrink_encoder *enc =
        heatshrink_encoder_alloc((uint8_t)window_size, (uint8_t)look_ahead);

    NAPI_CALL(
        env, napi_wrap(env, this, enc, encoder_instance_finalizer, NULL, NULL));
  }

  return this;
}

static napi_value encoder_method_sink(napi_env env, napi_callback_info info) {
  heatshrink_encoder *hse;

  void *data = NULL;
  size_t length;

  NAPI_ARGS(1);

  napi_value result;
  napi_valuetype rtype;
  NAPI_CALL(env, napi_typeof(env, argv[0], &rtype));

  if (rtype != napi_object) {
    NAPI_CALL(env, napi_create_int32(env, -1, &result));
    return result;
  }

  NAPI_CALL(env, napi_get_buffer_info(env, argv[0], &data, &length));

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hse));

  size_t bytes;
  HSE_sink_res sr = heatshrink_encoder_sink(hse, data, length, &bytes);

  if (sr == HSER_SINK_OK) {
    NAPI_CALL(env, napi_create_int32(env, (int32_t)bytes, &result));
  } else {
    NAPI_CALL(env, napi_create_int32(env, -1, &result));
  }
  return result;
}

static napi_value encoder_method_poll(napi_env env, napi_callback_info info) {
  heatshrink_encoder *hse;

  napi_value result;
  NAPI_THIS;

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hse));

  uint8_t data[256];
  size_t max = sizeof(data);
  size_t bytes = 0;

  HSE_poll_res res = HSER_POLL_MORE;
  while (res == HSER_POLL_MORE && bytes < max) {
    size_t inlen;
    res = heatshrink_encoder_poll(hse, data + bytes, max - bytes, &inlen);
    if (res >= 0)
      bytes += inlen;
    else
      break;
  }
  if (res == HSER_POLL_EMPTY || res == HSER_POLL_MORE) {
    void *ptr;
    napi_create_buffer_copy(env, bytes, (void *)data, &ptr, &result);
  } else {
    return NULL;
  }

  return result;
}

static napi_value encoder_method_end(napi_env env, napi_callback_info info) {
  heatshrink_encoder *hse;

  napi_value result;
  NAPI_THIS;

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hse));

  HSE_finish_res res = heatshrink_encoder_finish(hse);
  NAPI_CALL(env, napi_create_int32(env, res, &result));
  return result;
}

static napi_value decoder_constructor(napi_env env, napi_callback_info info) {

  NAPI_ARGS(1);

  napi_valuetype argType;
  NAPI_CALL(env, napi_typeof(env, argv[0], &argType));
  if (argType == napi_object) {

    // windowSize: number;
    // lookaheadSize: number;

    int32_t window_size;
    napi_value winsz;
    NAPI_CALL(env, napi_get_named_property(env, argv[0], "windowSize", &winsz));
    NAPI_CALL(env, napi_get_value_int32(env, winsz, &window_size));

    int32_t look_ahead;
    napi_value lahead;
    NAPI_CALL(env,
              napi_get_named_property(env, argv[0], "lookaheadSize", &lahead));
    NAPI_CALL(env, napi_get_value_int32(env, lahead, &look_ahead));

    heatshrink_decoder *dec =
        heatshrink_decoder_alloc((unsigned short)(1 << window_size),
                                 (uint8_t)window_size, (uint8_t)look_ahead);

    NAPI_CALL(
        env, napi_wrap(env, this, dec, decoder_instance_finalizer, NULL, NULL));
  }

  return this;
}

static napi_value decoder_method_sink(napi_env env, napi_callback_info info) {
  heatshrink_decoder *hsd;

  void *data = NULL;
  size_t length;

  NAPI_ARGS(1);

  napi_value result;
  napi_valuetype rtype;
  NAPI_CALL(env, napi_typeof(env, argv[0], &rtype));

  if (rtype != napi_object) {
    printf("DECODER - Not an Object. Returning error\n");
    NAPI_CALL(env, napi_create_int32(env, -1, &result));
    return result;
  }

  NAPI_CALL(env, napi_get_buffer_info(env, argv[0], &data, &length));

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hsd));

  size_t bytes;
  HSD_sink_res sr = heatshrink_decoder_sink(hsd, data, length, &bytes);

  if (sr == HSDR_SINK_OK) {
    NAPI_CALL(env, napi_create_int32(env, (int32_t)bytes, &result));
  } else {
    NAPI_CALL(env, napi_create_int32(env, -1, &result));
  }
  return result;
}

static napi_value decoder_method_poll(napi_env env, napi_callback_info info) {
  heatshrink_decoder *hsd;

  napi_value result;
  NAPI_THIS;

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hsd));

  uint8_t data[256];
  size_t max = sizeof(data);
  size_t bytes = 0;

  HSD_poll_res res = HSDR_POLL_MORE;
  while (res == HSDR_POLL_MORE && bytes < max) {
    size_t inlen;
    res = heatshrink_decoder_poll(hsd, data + bytes, max - bytes, &inlen);
    if (res >= 0)
      bytes += inlen;
    else
      break;
  }
  if (res == HSDR_POLL_EMPTY || res == HSDR_POLL_MORE) {
    void *ptr;
    napi_create_buffer_copy(env, bytes, (void *)data, &ptr, &result);
  } else {
    return NULL;
  }

  return result;
}

static napi_value decoder_method_end(napi_env env, napi_callback_info info) {
  heatshrink_decoder *hsd;

  napi_value result;
  NAPI_THIS;

  NAPI_CALL(env, napi_unwrap(env, this, (void **)&hsd));

  HSD_finish_res res = heatshrink_decoder_finish(hsd);
  NAPI_CALL(env, napi_create_int32(env, res, &result));
  return result;
}

napi_value create_addon(napi_env env) {
  napi_value result;
  NAPI_CALL(env, napi_create_object(env, &result));

  napi_value encoderClass;
  napi_value decoderClass;

  napi_property_descriptor encoderProperties[] = {
      {"sink", NULL, encoder_method_sink, NULL, NULL, NULL, napi_default, NULL},
      {"poll", NULL, encoder_method_poll, NULL, NULL, NULL, napi_default, NULL},
      {"end", NULL, encoder_method_end, NULL, NULL, NULL, napi_default, NULL}};

  napi_property_descriptor decoderProperties[] = {
      {"sink", NULL, decoder_method_sink, NULL, NULL, NULL, napi_default, NULL},
      {"poll", NULL, decoder_method_poll, NULL, NULL, NULL, napi_default, NULL},
      {"end", NULL, decoder_method_end, NULL, NULL, NULL, napi_default, NULL}};

  NAPI_CALL(env, napi_define_class(env, "Encoder", NAPI_AUTO_LENGTH,
                                   encoder_constructor, NULL, 3,
                                   encoderProperties, &encoderClass));

  NAPI_CALL(env, napi_define_class(env, "Decoder", NAPI_AUTO_LENGTH,
                                   decoder_constructor, NULL, 3,
                                   decoderProperties, &decoderClass));

  NAPI_CALL(env,
            napi_set_named_property(env, result, "HSEncoder", encoderClass));

  NAPI_CALL(env,
            napi_set_named_property(env, result, "HSDecoder", decoderClass));

  return result;
}
