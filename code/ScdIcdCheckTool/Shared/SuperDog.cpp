#include "StdAfx.h"
#include "SuperDog.h"
#include "dog_api.h"

unsigned char vendor_code[] =
	"V7yYXMPicC/AIOjsU1IltXUWgcQ6M2N84jWIvOl8ZriO39ChXd3fGQ62StIg/"
	"yu57rfG8YJxQLnsSq39Vi2iMW4Uc0GygXdXRAgvzamh7RPBYS+s3ef1y4ysC8"
	"qzodfmN99Um30VfmTUOu1o3XK/sZBC4GBdF7S7gRxW3D5bMmFFyhUiggSrPLY"
	"1o5QGkY6BS17QgLvu4kCeBfF8u77WO/BWP7N1kLaRiJDxci8vNIeiJNdtjFKt"
	"/8PA85DpUoWxevGUcAAKplACNLW8cWBPze248635KN4l+YP33K/+ZENsOM5Gc"
	"RhQ7zsNyVb+fwGY4GxehDxMeuTvJP9AL5KxnkEqZKE2g/fUcI94jJyjyWwPcQ"
	"WaAGMaFvpxSTmshatYJ55gUKdqhn/n//YKX3gpDUKnDNldXv2kJRylOZ2wmmf"
	"t7fBhpWXHnI/upbPJ0q6xFE7279r4tRgRNVW3/k0VbEBRlNt5zUMBzBFmlxtm"
	"emaVByI8YqvAFLkzfKUVeVMJNF5SWVHkxv9uv56GImcNGnKhC7CQC+Uh8Sa+L"
	"Q2ehUkAXCWzCkUtoWgh8Nlm9/M0VP9A25xl2KlEMKqquIB3kA16IXYeGn/IPm"
	"LRww0c/3WVcvhtYQXL3t7F1ItWQLFRKG07jWofk5brXBCjCqBjQcROPffILdw"
	"UVKIWVhTpbXfJty8I+5uUsQuyAW6KksA4LBbM2cav+23ddR63FPHzCg77Mq/Z"
	"pY33atGWOMGyE1BXm4HX2MQCCwVvp13olJA6dk6A/991SpTLwqPaDIZz1ltsz"
	"GA6MaJiWx7Gam7lord7POHQlrwTlB1aKAYnllUj2NGQ0YNCgmT6u9UNcLYDtD"
	"Re8V8sOfRTBjUs+pAhWocBqfLecymjNx0vD1RdZZcci2jL9/NF1LUFcNj5vCI"
	"IyKLJ7G1KZtEauNifmzAToIfbEees7HpczffW4IVfizHtxYG/frON3gVOSnjT6tadTA==";

#define CUSTOM_FEATUREID 2010
#define CUSTOM_FILEID    2007 
#define MEMBUFFER_SIZE   128
unsigned char membuffer[MEMBUFFER_SIZE];

bool CheckSuperDog()
{
	dog_status_t   status;
    dog_handle_t   handle;
    dog_size_t     fsize;

	status = dog_login(CUSTOM_FEATUREID,
                    (dog_vendor_code_t *)vendor_code,
                    &handle);

    switch (status)
    {
        case DOG_STATUS_OK:
            printf("OK\n");
            break;

        case DOG_FEATURE_NOT_FOUND:
            printf("login to default feature failed\n");
            break;

        case DOG_NOT_FOUND:
            printf("no SuperDog with vendor code DEMOMA found\n");
            break;

        case DOG_INV_VCODE:
            printf("invalid vendor code\n");
            break;

        case DOG_LOCAL_COMM_ERR:
            printf("communication error between API and local SuperDog License Manager\n");
            break;

        default:
            printf("login to default feature failed with status %d\n", status);
    }
    if (status) {
        return false;
    }

	status = dog_get_size(handle,
                    CUSTOM_FILEID,
                    &fsize);

    switch (status)
    {
        case DOG_STATUS_OK:
            printf("data file size is %d bytes\n", fsize);
            break;

        case DOG_INV_HND:
            printf("handle not active\n");
            break;

        case DOG_INV_FILEID:
            printf("invalid file id\n");
            break;

        case DOG_NOT_FOUND:
            printf("key/license container not available\n");
            break;

        default:
            printf("could not retrieve memory size\n");
    }
    if (status) {
        dog_logout(handle);
        return false;
    }
    
    if (fsize != 0)       /* skip memory access if no memory available */
    {
        if (fsize > MEMBUFFER_SIZE)
            fsize = MEMBUFFER_SIZE;

        printf("\nreading %4d bytes from memory   : ", fsize);

        status = dog_read(handle,
                       CUSTOM_FILEID,     /* file ID */
                       0,                 /* offset */
                       fsize,             /* length */
                       &membuffer[0]);    /* file data */

        switch (status)
        {
            case DOG_STATUS_OK:
                printf("OK\n");
               // dump(membuffer, fsize, "    ");
                break;

            case DOG_INV_HND:
                printf("handle not active\n");
                break;

            case DOG_INV_FILEID:
                printf("invalid file id\n");
                break;

            case DOG_MEM_RANGE:
                printf("exceeds data file range\n");
                break;

            case DOG_NOT_FOUND:
                printf("key/license container not available\n");
                break;

            default:
                printf("read memory failed\n");
        }
        if (status) {
            dog_logout(handle);
            return false;
        }
		char *Buffer = (char*)membuffer;
		if (strcmp(Buffer, "ROOF-SSDS") != 0)
		{
			return false;
		}
    } 
	return true;
}