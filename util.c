#include "main.h"

void* file_raw(char *path, uint32_t *size)
{
    FILE *file;
    char *data;
    int len;

    file = fopen(path, "rb");
    if(!file) {
        debug("File not found (%s)\n", path);
        return NULL;
    }

    fseek(file, 0, SEEK_END);
    len = ftell(file);
    data = malloc(len);
    if(!data) {
        fclose(file);
        return NULL;
    }

    fseek(file, 0, SEEK_SET);

    if(fread(data, len, 1, file) != 1) {
        debug("Read error (%s)\n", path);
        fclose(file);
        free(data);
        return NULL;
    }

    fclose(file);

    debug("Read %u bytes (%s)\n", len, path);

    if(size) {
        *size = len;
    }
    return data;
}

static void to_hex(char_t *a, char_t *p, int size)
{
    char_t b, c, *end = p + size;

    while(p != end) {
        b = *p++;

        c = (b & 0xF);
        b = (b >> 4);

        if(b < 10) {
            *a++ = b + '0';
        } else {
            *a++ = b - 10 + 'A';
        }

        if(c < 10) {
            *a++ = c + '0';
        } else {
            *a++ = c  - 10 + 'A';
        }
    }
}

void id_to_string(char_t *dest, char_t *src)
{
    to_hex(dest, src, TOX_FRIEND_ADDRESS_SIZE);
}

void cid_to_string(char_t *dest, char_t *src)
{
    to_hex(dest, src, TOX_CLIENT_ID_SIZE);
}

_Bool string_to_id(char_t *w, char_t *a)
{
    char_t *end = w + TOX_FRIEND_ADDRESS_SIZE;
    while(w != end) {
        char_t c, v;

        c = *a++;
        if(c >= '0' && c <= '9') {
            v = (c - '0') << 4;
        } else if(c >= 'A' && c <= 'F') {
            v = (c - 'A' + 10) << 4;
        } else {
            return 0;
        }

        c = *a++;
        if(c >= '0' && c <= '9') {
            v |= (c - '0');
        } else if(c >= 'A' && c <= 'F') {
            v |= (c - 'A' + 10);
        } else {
            return 0;
        }

        *w++ = v;
    }

    return 1;
}

int sprint_bytes(uint8_t *dest, uint64_t bytes)
{
    char *str[] = {"B", "KiB", "MiB", "GiB"};
    int i = 0;
    double f = bytes;
    while(bytes >= 1024) {
        bytes /= 1024;
        f /= 1024.0;
        i++;
    }

    int r;

    r = sprintf((char*)dest, "%u", (uint32_t)bytes);
    //missing decimals
    r += sprintf((char*)dest + r, "%s", str[i]);
    return r;
}

uint8_t utf8_len(char_t *data)
{
    if(!(*data & 0x80)) {
        return 1;
    }

    uint8_t bytes = 1, i;
    for(i = 6; i != 0xFF; i--) {
        if (!((*data >> i) & 1)) {
            break;
        }
        bytes++;
    }
    //no validation, instead validate all utf8 when recieved
    return bytes;
}

uint8_t utf8_len_read(char_t *data, uint32_t *ch)
{
    uint8_t a = data[0];
    if(!(a & 0x80)) {
        *ch = data[0];
        return 1;
    }

    if(!(a & 0x20)) {
        *ch = ((data[0] & 0x1F) << 6) | (data[1] & 0x3F);
        return 2;
    }

    if(!(a & 0x10)) {
        *ch =  ((data[0] & 0xF) << 12) | ((data[1] & 0x3F) << 6) | (data[2] & 0x3F);
        return 3;
    }

    if(!(a & 8)) {
        *ch =  ((data[0] & 0x7) << 18) | ((data[1] & 0x3F) << 12) | ((data[2] & 0x3F) << 6) | (data[3] & 0x3F);
        return 4;
    }

    if(!(a & 4)) {
        *ch =  ((data[0] & 0x3) << 24) | ((data[1] & 0x3F) << 18) | ((data[2] & 0x3F) << 12) | ((data[3] & 0x3F) << 6) | (data[4] & 0x3F);
        return 5;
    }

    if(!(a & 2)) {
        *ch =  ((data[0] & 0x1) << 30) | ((data[1] & 0x3F) << 24) | ((data[2] & 0x3F) << 18) | ((data[3] & 0x3F) << 12) | ((data[4] & 0x3F) << 6) | (data[5] & 0x3F);
        return 6;
    }

    //never happen
    return 0;
}

uint8_t utf8_unlen(char_t *data)
{
    uint8_t len = 1;
    if(*(data - 1) & 0x80) {
        do {
            len++;
        } while (!(*(data - len) & 0x40));
    }

    return len;
}

int utf8_validate(const char_t *data, int len)
{
    //stops when an invalid character is reached
    const char_t *a = data, *end = data + len;
    while(a != end) {
        if(!(*a & 0x80)) {
            a++;
            continue;
        }

        uint8_t bytes = 1, i;
        for(i = 6; i != 0xFF; i--) {
            if (!((*a >> i) & 1)) {
                break;
            }
            bytes++;
        }

        if(bytes == 1 || bytes == 8) {
            break;
        }

        // Validate the utf8
        if(a + bytes > end) {
            break;
        }

        for(i = 1; i < bytes; i++) {
            if(!(a[i] & 0x80) || (a[i] & 0x40)) {
                return a - data;
            }
        }

        a += bytes;
    }

    return a - data;
}

uint8_t unicode_to_utf8_len(uint32_t ch)
{
    if (ch > 0x1FFFFF) {
        return 0;
    }
    return 4 - (ch <= 0xFFFF) - (ch <= 0x7FF) - (ch <= 0x7F);
}

void unicode_to_utf8(uint32_t ch, char_t *dst)
{
    uint32_t HB = (uint32_t)0x80;
    uint32_t SB = (uint32_t)0x3F;
    if (ch <= 0x7F) {
        dst[0] = (uint8_t)ch;
        return;//1;
    }
    if (ch <= 0x7FF) {
        dst[0] = (uint8_t)((ch >> 6) | (uint32_t)0xC0);
        dst[1] = (uint8_t)((ch & SB) | HB);
        return;//2;
    }
    if (ch <= 0xFFFF) {
        dst[0] = (uint8_t)((ch >> 12) | (uint32_t)0xE0);
        dst[1] = (uint8_t)(((ch >> 6) & SB) | HB);
        dst[2] = (uint8_t)((ch & SB) | HB);
        return;//3;
    }
    if (ch <= 0x1FFFFF) {
        dst[0] = (uint8_t)((ch >> 18) | (uint32_t)0xF0);
        dst[1] = (uint8_t)(((ch >> 12) & SB) | HB);
        dst[2] = (uint8_t)(((ch >> 6) & SB) | HB);
        dst[3] = (uint8_t)((ch & SB) | HB);
        return;//4;
    }
    return;// 0;
}

uint8_t* tohtml(uint8_t *str, uint16_t length)
{
    int i = 0, len = 0;
    while(i != length) {
        switch(str[i]) {
        case '<':
        case '>': {
            len += 3;
            break;
        }

        case '&': {
            len += 4;
            break;
        }
        }

        i += utf8_len(str + i);
    }

    uint8_t *out = malloc(length + len + 1);
    i = 0; len = 0;
    while(i != length) {
        switch(str[i]) {
        case '<':
        case '>': {
            memcpy(out + len, str[i] == '>' ? "&gt;" : "&lt;", 4);
            len += 4;
            i++;
            break;
        }

        case '&': {
            memcpy(out + len, "&amp;", 5);
            len += 5;
            i++;
            break;
        }

        default: {
            int r = utf8_len(str + i);
            memcpy(out + len, str + i, r);
            len += r;
            i += r;
            break;
        }

        }
    }

    out[len] = 0;

    return out;
}

void yuv420torgb(vpx_image_t *img, uint8_t *out)
{
    unsigned long int i, j;
    for (i = 0; i < img->d_h; ++i) {
        for (j = 0; j < img->d_w; ++j) {
            uint8_t *point = out + 4 * ((i * img->d_w) + j);
            int y = img->planes[0][((i * img->stride[0]) + j)];
            int u = img->planes[1][(((i / 2) * img->stride[1]) + (j / 2))];
            int v = img->planes[2][(((i / 2) * img->stride[2]) + (j / 2))];

            int r = (298 * (y - 16) + 409 * (v - 128) + 128) >> 8;
            int g = (298 * (y - 16) - 100 * (u - 128) - 208 * (v - 128) + 128) >> 8;
            int b = (298 * (y - 16) + 516 * (u - 128) + 128) >> 8;
            point[0] = r>255? 255 : r<0 ? 0 : r;
            point[1] = g>255? 255 : g<0 ? 0 : g;
            point[2] = b>255? 255 : b<0 ? 0 : b;
            point[3] = ~0;
        }
    }
}

void yuv422to420(uint8_t *plane_y, uint8_t *plane_u, uint8_t *plane_v, uint8_t *input, uint16_t width, uint16_t height)
{
    uint8_t *end = input + width * height * 2;
    while(input != end) {
        uint8_t *line_end = input + width * 2;
        while(input != line_end) {
            *plane_y++ = *input++;
            *plane_v++ = *input++;
            *plane_y++ = *input++;
            *plane_u++ = *input++;
        }

        line_end = input + width * 2;
        while(input != line_end) {
            *plane_y++ = *input++;
            input++;//u
            *plane_y++ = *input++;
            input++;//v
        }

    }
}

void rgbtoyuv420(uint8_t *plane_y, uint8_t *plane_u, uint8_t *plane_v, uint8_t *rgb, uint16_t width, uint16_t height)
{
    uint16_t x, y;
    uint8_t *p;
    uint8_t r, g, b;

    for(y = 0; y != height; y += 2) {
        p = rgb;
        for(x = 0; x != width; x++) {
            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
        }

        for(x = 0; x != width / 2; x++) {
            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

            r = ((int)r + (int)*(rgb - 6) + (int)*p + (int)*(p + 3) + 2) / 4; p++;
            g = ((int)g + (int)*(rgb - 5) + (int)*p + (int)*(p + 3) + 2) / 4; p++;
            b = ((int)b + (int)*(rgb - 4) + (int)*p + (int)*(p + 3) + 2) / 4; p++;

            *plane_u++ = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
            *plane_v++ = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

            p += 3;
        }
    }
}

void rgbxtoyuv420(uint8_t *plane_y, uint8_t *plane_u, uint8_t *plane_v, uint8_t *rgb, uint16_t width, uint16_t height)
{
    uint16_t x, y;
    uint8_t *p;
    uint8_t r, g, b;

    for(y = 0; y != height; y += 2) {
        p = rgb;
        for(x = 0; x != width; x++) {
            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            rgb++;

            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;
        }

        for(x = 0; x != width / 2; x++) {
            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            rgb++;

            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

            r = *rgb++;
            g = *rgb++;
            b = *rgb++;
            rgb++;

            *plane_y++ = ((66 * r + 129 * g + 25 * b) >> 8) + 16;

            r = ((int)r + (int)*(rgb - 8) + (int)*p + (int)*(p + 4) + 2) / 4; p++;
            g = ((int)g + (int)*(rgb - 7) + (int)*p + (int)*(p + 4) + 2) / 4; p++;
            b = ((int)b + (int)*(rgb - 6) + (int)*p + (int)*(p + 4) + 2) / 4; p++;
            p++;

            *plane_u++ = ((-38 * r + -74 * g + 112 * b) >> 8) + 128;
            *plane_v++ = ((112 * r + -94 * g + -18 * b) >> 8) + 128;

            p += 4;
        }
    }
}

void scale_rgbx_image(uint8_t *old_rgbx, uint16_t old_width, uint16_t old_height, uint8_t *new_rgbx, uint16_t new_width, uint16_t new_height)
{
    int x, y, i;
    int x0, y0, x1, y1;

    float xf, yf;
    float dxf, dyf;
    float dx, dy;
    dxf = (float)old_width / new_width;
    dyf = (float)old_height / new_height;

    for(y = 0, yf = 0; y != new_height; y++, yf += dyf) {
        y0 = (int)yf;
        y1 = y0+1<old_height ? y0+1 : old_height-1;
        dy = yf - y0;

        for(x = 0, xf = 0; x != new_width; x++, xf += dxf) {
            x0 = (int)xf;
            x1 = x0+1<old_width ? x0+1 : old_width-1;
            dx = xf - x0;

            for(i = 0; i < 3; i++) {
                new_rgbx[(x+y*new_width)*4+i]
                  = old_rgbx[(x0 + y0*old_width)*4+i]*(1-dx)*(1-dy)
                  + old_rgbx[(x1 + y0*old_width)*4+i]*(  dx)*(1-dy)
                  + old_rgbx[(x0 + y1*old_width)*4+i]*(1-dx)*(  dy)
                  + old_rgbx[(x1 + y1*old_width)*4+i]*(  dx)*(  dy)
                  ;
                ;
            }
        }
    }
}
