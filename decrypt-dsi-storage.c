#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <TO.h>

#define PARTITION_NAME_SIZE 20
#define MAPPER_NAME_SIZE 10

static void print_usage (char *prg)
{
    fprintf(stderr, "Usage: %s <partition_name> <mapper_name>", prg);
    fprintf(stderr, "Exemple : ./%s /dev/mmcblk2p2 dsi", prg);
    fprintf(stderr, "\n");
}

int main(int argc, char** argv)
{
    FILE* pFile = NULL;
    int ret = -99;
    const uint8_t key_index = 1;
    const char *secret = "Secret#Key#For#HMAC#with#TO136";
    const int len = strlen(secret);
    uint8_t hmac_result[TO_HMAC_SIZE];
    memset(hmac_result, 0, TO_HMAC_SIZE);
    char partition[PARTITION_NAME_SIZE];
    char mapper[MAPPER_NAME_SIZE];
    char cmd[128];

    if (geteuid() != 0)
    {
        fprintf(stderr, "This program have to be run as root\n");
        ret = -1;
        goto err;
    }

    if (argc != 3)
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
        l = strlen(argv[2]);
        if (l > MAPPER_NAME_SIZE) l= MAPPER_NAME_SIZE;
        memcpy(mapper, argv[2], l);
        mapper[l] = '\0';
        snprintf(cmd, sizeof(cmd), "cryptsetup luksOpen %s %s --key-file /tmp/dsikey.bin && rm -f /tmp/dsikey.bin", partition, mapper);
    }

    int TO_status = TO_init();
    if ((TO_OK == TO_status) && (TORSP_SUCCESS == TO_compute_hmac(key_index, secret, len, hmac_result)))
    {
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
    }
    else
    {
        system("echo '#UnE.PhRaSe.QuElCoNQuE.PoUr.AvOiR.Un.LoNg.MoT.De.PaSsE!' | md5sum > /tmp/dsikey.bin");
        system(cmd);
    }
    ret = 0;
err:
    if (NULL != pFile) fclose(pFile);
    system("rm -f /tmp/dsikey.bin");
    if (TO_OK == TO_status) TO_fini();
    return ret;
}
