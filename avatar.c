#include "main.h"

/* gets avatar filepath for given client id string and stores it in dest,
 * returns number of chars written */
int get_avatar_location(char_t *dest, const char_t *id)
{
    char_t *p = dest + datapath_subdir(dest, AVATAR_DIRECTORY);
    memcpy((char *)p, id, TOX_CLIENT_ID_SIZE * 2); p += TOX_CLIENT_ID_SIZE * 2;
    strcpy((char *)p, ".png"); p += sizeof(".png") - 1;

    return p - dest;
}

/* gets avatar hash filepath for given client id string and stores it in dest,
 * returns number of chars written */
int get_avatar_hash_location(char_t *dest, const char_t *id)
{
    char_t *p = dest + datapath_subdir(dest, AVATAR_DIRECTORY);
    memcpy((char *)p, id, TOX_CLIENT_ID_SIZE * 2); p += TOX_CLIENT_ID_SIZE * 2;
    strcpy((char *)p, ".hash"); p += sizeof(".hash") - 1;

    return p - dest;
}

int load_avatar(const char_t *id, uint8_t *dest, uint32_t *size_out)
{
    char_t path[512];
    uint32_t size;

    get_avatar_location(path, id);

    uint8_t *avatar_data = file_raw((char *)path, &size);
    if (!avatar_data) {
        return 0;
    }
    if (size > TOX_AVATAR_MAX_DATA_LENGTH) {
        debug("warning: saved avatar file(%s) too large for tox\n", path);
        return 0;
    }

    memcpy(dest, avatar_data, size);
    free(avatar_data);
    if (size_out) {
        *size_out = size;
    }
    return 1;
}

int save_avatar(const char_t *id, const uint8_t *data, uint32_t size)
{
    char_t path[512];

    get_avatar_location(path, id);

    FILE *file = fopen((char*)path, "wb");
    if (file) {
        fwrite(data, size, 1, file);
        flush_file(file);
        fclose(file);
        return 1;
    } else {
        debug("error opening avatar file (%s) for writing\n", (char *)path);
        return 0;
    }
}

int delete_saved_avatar(const char_t *id)
{
    char_t path[512];

    get_avatar_location(path, id);

    return remove((char *)path);
}

int load_avatar_hash(const char_t *id, uint8_t *dest)
{
    char_t path[512];
    uint32_t size;

    get_avatar_hash_location(path, id);

    uint8_t *hash_data = file_raw((char *)path, &size);
    if (!hash_data) {
        return 0;
    }
    if (size != TOX_HASH_LENGTH) {
        debug("warning: saved avatar hash (%s) does not have TOX_HASH_LENGTH bytes\n", path);
        return 0;
    }

    memcpy(dest, hash_data, TOX_HASH_LENGTH);
    free(hash_data);
    return 1;
}

int save_avatar_hash(const char_t *id, uint8_t *hash)
{
    char_t path[512];

    get_avatar_hash_location(path, id);

    FILE *file = fopen((char*)path, "wb");
    if (file) {
        fwrite(hash, TOX_HASH_LENGTH, 1, file);
        flush_file(file);
        fclose(file);
        return 1;
    } else {
        debug("error opening avatar hash file (%s) for writing\n", (char *)path);
        return 0;
    }
}

int delete_avatar_hash(const char_t *id)
{
    char_t path[512];

    get_avatar_hash_location(path, id);

    return remove((char *)path);
}

int set_avatar(AVATAR *avatar, const uint8_t *data, uint32_t size, _Bool create_hash)
{
    if (size > TOX_AVATAR_MAX_DATA_LENGTH) {
        debug("warning: avatar too large\n");
        return 0;
    }

    uint16_t w, h;
    UTOX_NATIVE_IMAGE image = png_to_image((UTOX_PNG_IMAGE)data, size, &w, &h);
    if(!UTOX_NATIVE_IMAGE_IS_VALID(image)) {
        debug("warning: avatar is invalid\n");
        return 0;
    } else {
        avatar->image = image;
        avatar->width = w;
        avatar->height = h;
        avatar->format = TOX_AVATAR_FORMAT_PNG;
        if (create_hash) {
            tox_hash(avatar->hash, data, size);
        }
        return 1;
    }
}

void unset_avatar(AVATAR *avatar)
{
    avatar->format = TOX_AVATAR_FORMAT_NONE;
}

/* sets self avatar, see self_set_and_save_avatar */
int self_set_avatar(const uint8_t *data, uint32_t size)
{
    if (!set_avatar(&self.avatar, data, size, 1)) {
        return 0;
    }
    uint8_t *png_data = malloc(size);
    memcpy(png_data, data, size);
    tox_postmessage(TOX_SETAVATAR, TOX_AVATAR_FORMAT_PNG, size, png_data);
    return 1;
}

int self_set_and_save_avatar(const uint8_t *data, uint32_t size)
{
    if (self_set_avatar(data, size)) {
        save_avatar(self.id, data, size);
        return 1;
    } else {
        return 0;
    }
}

void self_remove_avatar()
{
    unset_avatar(&self.avatar);
    delete_saved_avatar(self.id);
    tox_postmessage(TOX_UNSETAVATAR, 0, 0, NULL);
}
