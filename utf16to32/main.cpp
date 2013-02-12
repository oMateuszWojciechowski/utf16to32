#include <endian.h>
#include <errno.h>
#include <iconv.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <wchar.h>

#define FROMCODE "UTF-16"

#if (BYTE_ORDER == LITTLE_ENDIAN)
#define TOCODE "UTF-32LE"
#elif (BYTE_ORDER == BIG_ENDIAN)
#define TOCODE "UTF-32BE"
#else
#error Unsupported byte order
#endif

int utf16to32(char *in, wchar_t **out, size_t insize){
	void *tmp=NULL;
	char *outbuf;
	char *inbuf;
	//char *output;
	long converted = 0;
	int n;
	size_t inbytesleft, outbytesleft, size;

    iconv_t cd = iconv_open(TOCODE, FROMCODE);
    if ((iconv_t)-1 == cd) {
        if (EINVAL == errno) {
            fprintf(stderr, "iconv: cannot convert from %s to %s\n",
                    FROMCODE, TOCODE);
        } else {
            fprintf(stderr, "iconv: %s\n", strerror(errno));
        }

        if (cd) {
            iconv_close(cd);
        }
        return -1;
    }
    size = insize * sizeof(wchar_t);
    inbuf = in;
    inbytesleft = insize;
    while (1) {
        tmp = realloc(*out, size + sizeof(wchar_t));
        if (!tmp) {
            fprintf(stderr, "realloc: %s\n", strerror(errno));


            if (cd) {
               iconv_close(cd);
            }
            return -1;
        }
        *out = (wchar_t *)tmp;
        outbuf = (char *)*out + converted;
        outbytesleft = size - converted;
        n = iconv(cd, (char **)&inbuf, &inbytesleft, &outbuf, &outbytesleft);
        if (-1 == n) {
            if (EINVAL == errno) {
                /* junk at the end of the buffer, ignore it */
                break;
            } else if (E2BIG != errno) {
                /* unrecoverable error */
                fprintf(stderr, "iconv: %s\n", strerror(errno));


                if (cd) {
                    iconv_close(cd);
                }
                return -1;
            }
            /* increase the size of the output buffer */
            converted = size - outbytesleft;
            size <<= 1;
        } else {
            /* done */
            break;
        }
    }
    converted = (size - outbytesleft) / sizeof(wchar_t);
    *out[converted] = L'\0';

    iconv(cd, NULL, NULL, &outbuf, &outbytesleft);
    //fprintf(stdout, "%ls\n", *out);

    if (cd) {
        iconv_close(cd);
    }
    return 0;
}

int main(void)
{

    wchar_t *out = new wchar_t();

    int a;
    char * inbuf;
    char in[] = {
        0xff, 0xfe,
        'H', 0x0,
        'e', 0x0,
        'l', 0x0,
        'l', 0x0,
        'o', 0x0,
        ',', 0x0,
        ' ', 0x0,
        'W', 0x0,
        'o', 0x0,
        'r', 0x0,
        'l', 0x0,
        'd', 0x0,
        '!', 0x0
    };
    inbuf=in;
   a= utf16to32(inbuf, &out, sizeof(in));
   fprintf(stdout, "%ls\n", out);
   free(out);
    return 0;

}
