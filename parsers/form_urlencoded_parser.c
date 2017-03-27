#include "er-http.h"
#include <stdlib.h>
#include <ctype.h>

#include "parser.h"


static char key[POST_PARAMS_KEY_MAX_LEN];
static char val_escaped[POST_PARAMS_VAL_MAX_LEN];
static char val[POST_PARAMS_VAL_MAX_LEN];
static int key_len;
static int val_len;
static long state;

#define DEBUG 1
#if DEBUG
#define PRINTF(...) printf(__VA_ARGS__)
#define PRINT6ADDR(addr) PRINTF("[%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x:%02x%02x]", ((uint8_t *)addr)[0], ((uint8_t *)addr)[1], ((uint8_t *)addr)[2], ((uint8_t *)addr)[3], ((uint8_t *)addr)[4], ((uint8_t *)addr)[5], ((uint8_t *)addr)[6], ((uint8_t *)addr)[7], ((uint8_t *)addr)[8], ((uint8_t *)addr)[9], ((uint8_t *)addr)[10], ((uint8_t *)addr)[11], ((uint8_t *)addr)[12], ((uint8_t *)addr)[13], ((uint8_t *)addr)[14], ((uint8_t *)addr)[15])
#define PRINTLLADDR(lladdr) PRINTF("[%02x:%02x:%02x:%02x:%02x:%02x]", (lladdr)->addr[0], (lladdr)->addr[1], (lladdr)->addr[2], (lladdr)->addr[3], (lladdr)->addr[4], (lladdr)->addr[5])
#else
#define PRINTF(...)
#define PRINT6ADDR(addr)
#define PRINTLLADDR(addr)
#endif

static size_t
url_unescape(const char *src, size_t srclen, char *dst, size_t dstlen)
{
  size_t i, j;
  int a, b;

  for(i = j = 0; i < srclen && j < dstlen - 1; i++, j++) {
    if(src[i] == '%' && isxdigit(*(unsigned char *)(src + i + 1))
       && isxdigit(*(unsigned char *)(src + i + 2))) {
      a = tolower(*(unsigned char *)(src + i + 1));
      b = tolower(*(unsigned char *)(src + i + 2));
      dst[j] = ((HEX_TO_INT(a) << 4) | HEX_TO_INT(b)) & 0xff;
      i += 2;
    } else if(src[i] == '+') {
      dst[j] = ' ';
    } else {
      dst[j] = src[i];
    }
  }

  dst[j] = '\0';

  return i == srclen;
}

size_t
parse_post_param(char *buf, int buf_len, p_parse_pair_t key_pair, int len_user)
{

  int i, counter = 0;
  int finish;
  state = PARSE_POST_STATE_INIT;

  for(i = 0; i < buf_len; i++) {
    switch(state) {
    case PARSE_POST_STATE_INIT:
      state = PARSE_POST_STATE_MORE;
    /* continue */
    case PARSE_POST_STATE_MORE:
      memset(key, 0, POST_PARAMS_KEY_MAX_LEN);
      memset(val, 0, POST_PARAMS_VAL_MAX_LEN);
      memset(val_escaped, 0, POST_PARAMS_VAL_MAX_LEN);
      key_len = 0;
      val_len = 0;
      state = PARSE_POST_STATE_READING_KEY;
    /* continue */
    case PARSE_POST_STATE_READING_KEY:
      if(buf[i] == ISO_equal) {
        state = PARSE_POST_STATE_READING_VAL;
      } else if(buf[i] == ISO_amp) {
        /* Don't accept an amp while reading a key */
        state = PARSE_POST_STATE_ERROR;
      } else {
        /* Make sure we don't overshoot key's boundary - Don't forget the null terminator */
        if(key_len < POST_PARAMS_KEY_MAX_LEN - 1) {
          key[key_len] = buf[i];
          key_len++;
        } else {
          /* Not enough space for the key. Abort */
          state = PARSE_POST_STATE_ERROR;
        }
      }
      break;
    case PARSE_POST_STATE_READING_VAL:
      finish = 0;
      if(buf[i] == ISO_amp) {
        finish = 1;
      } else if(buf[i] == ISO_equal) {
        /* Don't accept an '=' while reading a val */
        state = PARSE_POST_STATE_ERROR;
      } else {
        /* Make sure we don't overshoot key's boundary - Don't forget the null terminator */
        if(val_len < POST_PARAMS_VAL_MAX_LEN - 1) {
          val[val_len] = buf[i];
          val_len++;
          /* Last character of the last chunk */
          if(i == buf_len - 1) {
            finish = 1;
          }
        } else {
          /* Not enough space for the value. Abort */
          state = PARSE_POST_STATE_ERROR;
        }
      }

      if(finish == 1) {
        /*
         * Done reading a key=value pair, either because we encountered an amp
         * or because we reached the end of the message body.
         *
         * First, unescape the value.
         *
         * Then copy them. We will bail out with PARSE_POST_STATE_ERROR,
         * unless the key-val gets correctly processed
         */
    	  url_unescape(val, val_len, val_escaped, POST_PARAMS_VAL_MAX_LEN);
    	  val_len = strlen(val_escaped);
    	  memcpy(key_pair[counter].key, key, key_len);
    	  key_pair[counter].key[key_len] = '\0';

    	  memcpy(key_pair[counter].value, val_escaped, val_len);
    	  key_pair[counter].value[val_len] = '\0';

    	  counter++;
    	  len_user--;

    	  if(len_user < 0 && buf_len - 1 < i){
    		  state = PARSE_POST_STATE_ERROR;
    	  }
    	  else {
    		  state = PARSE_POST_STATE_MORE;
    	  }

      }
      break;
    case PARSE_POST_STATE_ERROR:
      /* If we entered the error state earlier, do nothing */
      return PARSE_POST_STATE_ERROR;
    default:
      break;
    }
  }

  return PARSE_POST_STATE_ERROR;
}

