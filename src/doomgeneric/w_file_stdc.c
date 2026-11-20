#include <stdio.h>
#include <string.h>

#include "m_misc.h"
#include "w_file.h"
#include "z_zone.h"

// wad_data.c で定義(フラッシュ上の const 配列)
extern const unsigned char doom_wad_data[];
extern const size_t doom_wad_len;

extern wad_file_class_t stdc_wad_file;

static wad_file_t *W_StdC_OpenFile(char *path)
{
    wad_file_t *result;

    (void)path; // どのパスで呼ばれても埋め込み WAD を返す

    result = Z_Malloc(sizeof(wad_file_t), PU_STATIC, 0);
    result->file_class = &stdc_wad_file;
    result->mapped = (byte *)doom_wad_data; // コピーせず直接参照
    result->length = (unsigned int)doom_wad_len;

    return result;
}

static void W_StdC_CloseFile(wad_file_t *wad)
{
    Z_Free(wad);
}

size_t W_StdC_Read(wad_file_t *wad, unsigned int offset,
                   void *buffer, size_t buffer_len)
{
    if (offset >= wad->length)
        return 0;

    if (buffer_len > wad->length - offset)
        buffer_len = wad->length - offset;

    memcpy(buffer, doom_wad_data + offset, buffer_len);
    return buffer_len;
}

wad_file_class_t stdc_wad_file =
    {
        W_StdC_OpenFile,
        W_StdC_CloseFile,
        W_StdC_Read,
};