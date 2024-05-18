#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <TO.h>

#define PARTITION_NAME_SIZE 20

static void print_usage (char *prg)
{
    fprintf(stderr, "Usage: %s <partition_name>", prg);
    fprintf(stderr, "Exemple : ./%s /dev/mmcblk2p2", prg);
    fprintf(stderr, "\n");
}

int main(int argc, char** argv)
{
    FILE* pFile = NULL;
    int ret = -99;
    const uint8_t key_index = 1;
    const char *secret = "Secret#Key#For#HMAC#with#TO135";
    const int len = strlen(secret);
    uint8_t hmac_result[TO_HMAC_SIZE];
    memset(hmac_result, 0, TO_HMAC_SIZE);
    char partition[PARTITION_NAME_SIZE];
    char cmd[128];

    if (geteuid() != 0)
    {
        fprintf(stderr, "This program have to be run as root\n");
        ret = -1;
        goto err;
    }

    if (argc != 2)
    {
        print_usage(argv[0]);
        ret = -2;
        goto err;
    }
    else
    {
        int l = strlen(argv[1]);
        if (l > PARTITION_NAME_SIZE) l= PARTITION_NAME_SIZE;
        memcpy(partition, argv[1], l);
        partition[l] = '\0';
        snprintf(cmd, sizeof(cmd), "cryptsetup -v -q luksFormat %s --type luks2 --key-file /tmp/dsikey.bin && rm -f /tmp/dsikey.bin", partition);
    }

    int TO_status = TO_init();
    if ((TO_OK == TO_status) && (TORSP_SUCCESS == TO_compute_hmac(key_index, secret, len, hmac_result)))
    {
        fprintf(stdout, "- TO136 -\n");

        pFile = fopen("/tmp/dsikey.bin", "wb");

        if (NULL != pFile)
        {
            if (TO_HMAC_SIZE != fwrite(hmac_result, sizeof(uint8_t), TO_HMAC_SIZE, pFile))
            {
                fprintf(stderr, "Unable to write\n");
                ret = -3;
                goto err;
            }
            fclose(pFile);
            pFile = NULL;
            system(cmd);
        }
        else
        {
            fprintf(stderr, "Unable to open\n");
            ret = -4;
            goto err;
        }

        pFile = fopen("/root/.to136", "w");

        if (NULL != pFile)
        {
            int i=0;
            uint8_t serial_number[TO_SN_SIZE];
            uint8_t product_number[TO_PN_SIZE];
            uint8_t hardware_version[TO_HW_VERSION_SIZE];
            memset(serial_number, 0, TO_SN_SIZE);
            memset(product_number, 0, TO_PN_SIZE);
            memset(hardware_version, 0, TO_HW_VERSION_SIZE);
            uint8_t major=0, minor=0, revision=0;

            if ((TORSP_SUCCESS != TO_get_serial_number(serial_number)) ||
                (TORSP_SUCCESS != TO_get_product_number(product_number)) ||
                (TORSP_SUCCESS != TO_get_hardware_version(hardware_version)) ||
                (TORSP_SUCCESS != TO_get_software_version(&major, &minor, &revision)))
            {
                fprintf(stderr, "Unable to get info\n");
                ret = -5;
                goto err;
            }

            for (i=0; i<TO_SN_SIZE; i++)
            {
                if (2 != fprintf(pFile, "%02X", serial_number[i]))
                {
                    fprintf(stderr, "Unable to write SN info\n");
                    ret = -6;
                    goto err;
                }
            }
            fprintf(pFile, "\n");

            for (i=0; i<TO_PN_SIZE; i++)
            {
                if (2 != fprintf(pFile, "%02X", product_number[i]))
                {
                    fprintf(stderr, "Unable to write PN info\n");
                    ret = -7;
                    goto err;
                }
            }
            fprintf(pFile, "\n");

            for (i=0; i<TO_HW_VERSION_SIZE; i++)
            {
                if (2 != fprintf(pFile, "%02X", hardware_version[i]))
                {
                    fprintf(stderr, "Unable to write HW info\n");
                    ret = -8;
                    goto err;
                }
            }
            fprintf(pFile, "\n");

            fprintf(pFile, "%u.%u.%u\n", (unsigned char)major, (unsigned char)minor, (unsigned char)revision);

            fclose(pFile);
            pFile = NULL;
        }
        else
        {
            fprintf(stderr, "Unable to open to136 file\n");
            ret = -9;
            goto err;
        }
    }
    else
    {
        fprintf(stdout, "- MD5 -\n");
        system("echo '#UnE.PhRaSe.QuElCoNQuE.PoUr.AvOiR.Un.LoNg.MoT.De.PaSsE!' | md5sum > /tmp/dsikey.bin");
        system(cmd);
        system("echo md5 > /root/.to136");
    }
    ret = 0;
err:
    if (NULL != pFile) fclose(pFile);
    system("rm -f /tmp/dsikey.bin");
    if (TO_OK == TO_status) TO_fini();
    return ret;
}
