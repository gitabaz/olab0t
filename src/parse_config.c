#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <stdbool.h>
#include <ctype.h>

#include "parse_config.h"

void free_bc(bot_config* bc) {
    for(int i = 0; i < bc->capacity; i++) {
        free(bc->channel_list[i]);
    }
    free(bc->channel_list);
    free(bc);
}

// TODO: change parsing to use strtok?
bot_config* bot_config_from_file(const char* const filename) {
    
    FILE* fp = fopen(filename, "r");
    
    if (!fp) {
        perror("Could not open config file");
        exit(EXIT_FAILURE);
    }

    bot_config* bc = malloc(sizeof(*bc));
    bc->size = 10; // Default storage for 10 channels
    bc->capacity = 0;
    bc->channel_list = malloc((bc->size) * sizeof(*(bc->channel_list)));

    char buf[101];
    int i = 0;
    bool val_is_array = false;
    bool flag_bu = false, flag_cl = false, flag_ot = false;
    while (fscanf(fp, "%100[^\"]%*c", buf) != EOF) {
        switch (i % 4) {
            case 0:
                // This is the pre key junk
                break;
            case 1:
                // This is the key
                if (strcmp(buf, "bot_username") == 0) {
                    flag_bu = true;
                } else if (strcmp(buf, "channel_list") == 0) {
                    flag_cl = true;
                } else if (strcmp(buf, "oauth_token") == 0) {
                    flag_ot = true;
                } else {
                    printf("Error parsing `%s`", buf);
                }
                break;
            case 2:
                // This is the post key junk
                // If the post key junk contains [ we are about to read an array
                // Otherwise we are reading a value
                if (strstr(buf, "[")) {
                    val_is_array = true;
                }
                break;
            case 3:
                // Read the value
                if (val_is_array) {
                    int j = 0;
                    do {
                        // If we read a ] we have, not only, finished reading
                        // the value array but also read the pre key junk. We need to
                        // update i by +1
                        if (strstr(buf, "]")) {
                            val_is_array = false;
                            flag_cl = false;
                            i += 1;
                            break;
                        } else {
                            switch (j % 2) {
                                case 0:
                                    if (flag_cl != true) {
                                        fprintf(stderr, "Error: `%s` does not belong in an array", buf);
                                        exit(EXIT_FAILURE);
                                    }
                                    if (bc->capacity < bc->size) {
                                        bc->channel_list[bc->capacity] = malloc((strlen(buf) + 1) * sizeof(*(bc->channel_list[bc->capacity])));
                                        strcpy(bc->channel_list[bc->capacity], buf);
                                        bc->capacity++;
                                    } else {
                                        printf("Error: only %d channels allowed. You requested `%d`", bc->size, bc->capacity);
                                        exit(EXIT_FAILURE);
                                    }
                                    break;
                                case 1:
                                    // This is post value junk 
                                    break;
                            }
                        }
                        j++;
                    } while (fscanf(fp, "%100[^\"]%*c", buf) != EOF);

                } else {
                    if (flag_bu == true) {
                        strcpy(bc->bot_username, buf);
                        flag_bu = false;
                    } else if (flag_cl == true) {
                        fprintf(stderr, "Error: channel list must be an array");
                        exit(EXIT_FAILURE);
                    } else if (flag_ot == true) {
                        strcpy(bc->oauth_token, buf);
                        flag_ot = false;
                    }
                }
                break;
        }
        i++;
    }

    fclose(fp);

    return bc;
}
