#include <stdio.h>
#include "addon.h"
#include "heatshrink_encoder.h"

#define NAPI_CALL(env, call)                                      \
  do {                                                            \
    napi_status status = (call);                                  \
    if (status != napi_ok) {                                      \
      const napi_extended_error_info* error_info = NULL;          \
      napi_get_last_error_info((env), &error_info);               \
      bool is_pending;                                            \
      napi_is_exception_pending((env), &is_pending);              \
      if (!is_pending) {                                          \
        const char* message = (error_info->error_message == NULL) \
            ? "empty error message"                               \
            : error_info->error_message;                          \
        napi_throw_error((env), NULL, message);                   \
        return NULL;                                              \
      }                                                           \
    }                                                             \
  } while(0)

static void
encoder_instance_finalizer(napi_env env, void *data, void *hint) {
	printf("Called finalizer..\n");

	printf("Instance data = %p\n",(char *)data);
	heatshrink_encoder_free((heatshrink_encoder *)data);
}

static napi_value
encoder_close_method(napi_env env, napi_callback_info info) {
	heatshrink_encoder *hse;

	napi_value this;
	napi_get_cb_info(env, info,
					 0,
					 NULL,
					 &this,
					 NULL);

	NAPI_CALL(env, napi_remove_wrap(env, this, (void **)&hse));

	heatshrink_encoder_free(hse);
	return NULL;
}

static napi_value
encoder_constructor(napi_env env, napi_callback_info info) {
	printf("encoder_constructor\n");

	size_t argc = 1;
	napi_value argv[1];
	napi_value this;

	napi_value _this;
	NAPI_CALL(env, napi_get_cb_info(env, info, NULL, NULL, &_this, NULL));

	napi_get_cb_info(env, info,
					 &argc,
					 argv,
					 &this,
					 NULL);


	napi_valuetype argType;
	NAPI_CALL(env, napi_typeof(env,argv[0], &argType));
	if( argType == napi_object ) {

		//windowSize: number;
		//lookaheadSize: number;

		int32_t window_size;
		napi_value winsz;
		NAPI_CALL(env,napi_get_named_property(env, argv[0], "windowSize", &winsz));
		NAPI_CALL(env, napi_get_value_int32(env, winsz, &window_size));

		int32_t look_ahead;
		napi_value lahead;
		NAPI_CALL(env,napi_get_named_property(env, argv[0], "lookaheadSize", &lahead));
		NAPI_CALL(env, napi_get_value_int32(env, lahead, &look_ahead));

		heatshrink_encoder *enc = heatshrink_encoder_alloc( (uint8_t) window_size, (uint8_t) look_ahead);

		NAPI_CALL(env, napi_wrap(env,
								 _this,
								 enc,
								 encoder_instance_finalizer,
								 NULL,
								 NULL
								));
	}

	return _this;
}

static napi_value
decoder_constructor(napi_env env, napi_callback_info info) {
	printf("decoder_constructor\n");
	return NULL;
}

static napi_value
example_method(napi_env env, napi_callback_info info) {
	printf("example_method\n");
	char *pdata;
	size_t argc = 2;
	napi_value argv[2];
	napi_value this;

	napi_get_cb_info(env, info,
					 &argc,
					 argv,
					 &this,
					 NULL);

	NAPI_CALL(env,napi_unwrap(env, this, &pdata));
	printf("pdata = %s\n",pdata);

	printf("argc = %ld\n",argc);
	napi_valuetype argType;
	NAPI_CALL(env, napi_typeof(env,argv[0], &argType));
	if( argType == napi_string ) {
		printf("type = STRING!!\n");

		size_t result;
		char buf[128];
		NAPI_CALL(env, napi_get_value_string_latin1(env,argv[0], buf, sizeof(buf), &result));
		printf("Result = %s (%ld bytes)\n",buf,result);
	}
	return NULL;
}

napi_value create_addon(napi_env env) {
	napi_value result;
	NAPI_CALL(env, napi_create_object(env, &result));

	napi_value encoderClass;
	napi_value decoderClass;

	napi_property_descriptor encoderProperties[] = {
		{
			"close",
			NULL,
			encoder_close_method,
			NULL,
			NULL,
			NULL,
			napi_default,
			NULL
		}
	};

	napi_property_descriptor decoderProperties[] = {
		{
			"close",
			NULL,
			example_method,
			NULL,
			NULL,
			NULL,
			napi_default,
			NULL
		}
	};

	NAPI_CALL(env, napi_define_class(env,
									 "Encoder",
									 NAPI_AUTO_LENGTH,
									 encoder_constructor,
									 NULL,
									 1,
									 encoderProperties,
									 &encoderClass));

	NAPI_CALL(env, napi_define_class(env,
									 "Decoder",
									 NAPI_AUTO_LENGTH,
									 decoder_constructor,
									 NULL,
									 1,
									 decoderProperties,
									 &decoderClass));

	NAPI_CALL(env, napi_set_named_property(env,
										   result,
										   "Encoder",
										   encoderClass));

	NAPI_CALL(env, napi_set_named_property(env,
										   result,
										   "Decoder",
										   decoderClass));

	return result;
}
