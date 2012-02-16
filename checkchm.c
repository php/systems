/* $Id$ */
/***************************************************************************
 *             chm_lib.c - CHM archive manipulation routines               *
 *                           -------------------                           *
 *                                                                         *
 *  author:     Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  version:    0.3                                                        *
 *  notes:      These routines are meant for the manipulation of microsoft *
 *              .chm (compiled html help) files, but may likely be used    *
 *              for the manipulation of any ITSS archive, if ever ITSS     *
 *              archives are used for any other purpose.                   *
 *                                                                         *
 *              Note also that the section names are statically handled.   *
 *              To be entirely correct, the section names should be read   *
 *              from the section names meta-file, and then the various     *
 *              content sections and the "transforms" to apply to the data *
 *              they contain should be inferred from the section name and  *
 *              the meta-files referenced using that name; however, all of *
 *              the files I've been able to get my hands on appear to have *
 *              only two sections: Uncompressed and MSCompressed.          *
 *              Additionally, the ITSS.DLL file included with Windows does *
 *              not appear to handle any different transforms than the     *
 *              simple LZX-transform.  Furthermore, the list of transforms *
 *              to apply is broken, in that only half the required space   *
 *              is allocated for the list.  (It appears as though the      *
 *              space is allocated for ASCII strings, but the strings are  *
 *              written as unicode.  As a result, only the first half of   *
 *              the string appears.)  So this is probably not too big of   *
 *              a deal, at least until CHM v4 (MS .lit files), which also  *
 *              incorporate encryption, of some description.               *
 *                                                                         *
 * switches:    CHM_MT:        compile library with thread-safety          *
 *                                                                         *
 * switches (Linux only):                                                  *
 *              CHM_USE_PREAD: compile library to use pread instead of     *
 *                             lseek/read                                  *
 *              CHM_USE_IO64:  compile library to support full 64-bit I/O  *
 *                             as is needed to properly deal with the      *
 *                             64-bit file offsets.                        *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

/* $Id$ */
/***************************************************************************
 *                        lzx.h - LZX decompression routines               *
 *                           -------------------                           *
 *                                                                         *
 *  maintainer: Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  source:     modified lzx.c from cabextract v0.5                        *
 *  notes:      This file was taken from cabextract v0.5, which was,       *
 *              itself, a modified version of the lzx decompression code   *
 *              from unlzx.                                                *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.  Note that an exemption to this   *
 *   license has been granted by Stuart Caie for the purposes of           *
 *   distribution with chmlib.  This does not, to the best of my           *
 *   knowledge, constitute a change in the license of this (the LZX) code  *
 *   in general.                                                           *
 *                                                                         *
 ***************************************************************************/

#ifndef INCLUDED_LZX_H
#define INCLUDED_LZX_H

#ifdef __cplusplus
extern "C" {
#endif

/* return codes */
#define DECR_OK           (0)
#define DECR_DATAFORMAT   (1)
#define DECR_ILLEGALDATA  (2)
#define DECR_NOMEMORY     (3)

/* opaque state structure */
struct LZXstate;

/* create an lzx state object */
struct LZXstate *LZXinit(int window);

/* destroy an lzx state object */
void LZXteardown(struct LZXstate *pState);

/* reset an lzx stream */
int LZXreset(struct LZXstate *pState);

/* decompress an LZX compressed block */
int LZXdecompress(struct LZXstate *pState,
                  unsigned char *inpos,
                  unsigned char *outpos,
                  int inlen,
                  int outlen);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_LZX_H */
/* $Id$ */
/***************************************************************************
 *             chm_lib.h - CHM archive manipulation routines               *
 *                           -------------------                           *
 *                                                                         *
 *  author:     Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  version:    0.3                                                        *
 *  notes:      These routines are meant for the manipulation of microsoft *
 *              .chm (compiled html help) files, but may likely be used    *
 *              for the manipulation of any ITSS archive, if ever ITSS     *
 *              archives are used for any other purpose.                   *
 *                                                                         *
 *              Note also that the section names are statically handled.   *
 *              To be entirely correct, the section names should be read   *
 *              from the section names meta-file, and then the various     *
 *              content sections and the "transforms" to apply to the data *
 *              they contain should be inferred from the section name and  *
 *              the meta-files referenced using that name; however, all of *
 *              the files I've been able to get my hands on appear to have *
 *              only two sections: Uncompressed and MSCompressed.          *
 *              Additionally, the ITSS.DLL file included with Windows does *
 *              not appear to handle any different transforms than the     *
 *              simple LZX-transform.  Furthermore, the list of transforms *
 *              to apply is broken, in that only half the required space   *
 *              is allocated for the list.  (It appears as though the      *
 *              space is allocated for ASCII strings, but the strings are  *
 *              written as unicode.  As a result, only the first half of   *
 *              the string appears.)  So this is probably not too big of   *
 *              a deal, at least until CHM v4 (MS .lit files), which also  *
 *              incorporate encryption, of some description.               *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#ifndef INCLUDED_CHMLIB_H
#define INCLUDED_CHMLIB_H

#ifdef __cplusplus
extern "C" {
#endif

#ifdef WIN32
typedef unsigned __int64 LONGUINT64;
typedef __int64          LONGINT64;
#else
typedef unsigned long long LONGUINT64;
typedef long long          LONGINT64;
#endif

/* the two available spaces in a CHM file                      */
/* N.B.: The format supports arbitrarily many spaces, but only */
/*       two appear to be used at present.                     */
#define CHM_UNCOMPRESSED (0)
#define CHM_COMPRESSED   (1)

/* structure representing an ITS (CHM) file stream             */
struct chmFile;

/* structure representing an element from an ITS file stream   */
#define CHM_MAX_PATHLEN  (256)
struct chmUnitInfo
{
    LONGUINT64         start;
    LONGUINT64         length;
    int                space;
    char               path[CHM_MAX_PATHLEN+1];
};

/* open an ITS archive */
struct chmFile* chm_open(const char *filename);

/* close an ITS archive */
void chm_close(struct chmFile *h);

/* methods for ssetting tuning parameters for particular file */
#define CHM_PARAM_MAX_BLOCKS_CACHED 0
void chm_set_param(struct chmFile *h,
                   int paramType,
                   int paramVal);

/* resolve a particular object from the archive */
#define CHM_RESOLVE_SUCCESS (0)
#define CHM_RESOLVE_FAILURE (1)
int chm_resolve_object(struct chmFile *h,
                       const char *objPath,
                       struct chmUnitInfo *ui);

/* retrieve part of an object from the archive */
LONGINT64 chm_retrieve_object(struct chmFile *h,
                              struct chmUnitInfo *ui,
                              unsigned char *buf,
                              LONGUINT64 addr,
                              LONGINT64 len);

/* enumerate the objects in the .chm archive */
typedef int (*CHM_ENUMERATOR)(struct chmFile *h,
                              struct chmUnitInfo *ui,
                              void *context);
#define CHM_ENUMERATE_NORMAL    (1)
#define CHM_ENUMERATE_META      (2)
#define CHM_ENUMERATE_SPECIAL   (4)
#define CHM_ENUMERATE_FILES     (8)
#define CHM_ENUMERATE_DIRS      (16)
#define CHM_ENUMERATE_ALL       (31)
#define CHM_ENUMERATOR_FAILURE  (0)
#define CHM_ENUMERATOR_CONTINUE (1)
#define CHM_ENUMERATOR_SUCCESS  (2)
int chm_enumerate(struct chmFile *h,
                  int what,
                  CHM_ENUMERATOR e,
                  void *context);

int chm_enumerate_dir(struct chmFile *h,
                      const char *prefix,
                      int what,
                      CHM_ENUMERATOR e,
                      void *context);

#ifdef __cplusplus
}
#endif

#endif /* INCLUDED_CHMLIB_H */

#ifdef CHM_MT
#define _REENTRANT
#endif

#include <stdlib.h>
#include <string.h>

#if __sun || __sgi
#include <strings.h>
#endif

#ifdef WIN32
#include <windows.h>
#include <malloc.h>
#else
/* basic Linux system includes */
#define _XOPEN_SOURCE 500
#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
/* #include <dmalloc.h> */
#endif

/* includes/defines for threading, if using them */
#ifdef CHM_MT
#ifdef WIN32
#define CHM_ACQUIRE_LOCK(a) do {                        \
        EnterCriticalSection(&(a));                     \
    } while(0)
#define CHM_RELEASE_LOCK(a) do {                        \
        EnterCriticalSection(&(a));                     \
    } while(0)

#else
#include <pthread.h>

#define CHM_ACQUIRE_LOCK(a) do {                        \
        pthread_mutex_lock(&(a));                       \
    } while(0)
#define CHM_RELEASE_LOCK(a) do {                        \
        pthread_mutex_unlock(&(a));                     \
    } while(0)

#endif
#else
#define CHM_ACQUIRE_LOCK(a) /* do nothing */
#define CHM_RELEASE_LOCK(a) /* do nothing */
#endif

#ifdef WIN32
#define CHM_NULL_FD (INVALID_HANDLE_VALUE)
#define CHM_USE_WIN32IO 1
#define CHM_CLOSE_FILE(fd) CloseHandle((fd))
#else
#define CHM_NULL_FD (-1)
#define CHM_CLOSE_FILE(fd) close((fd))
#endif

/*
 * defines related to tuning
 */
#ifndef CHM_MAX_BLOCKS_CACHED
#define CHM_MAX_BLOCKS_CACHED 5
#endif

/*
 * architecture specific defines
 *
 * Note: as soon as C99 is more widespread, the below defines should
 * probably just use the C99 sized-int types.
 *
 * The following settings will probably work for many platforms.  The sizes
 * don't have to be exactly correct, but the types must accommodate at least as
 * many bits as they specify.
 */

/* i386, 32-bit, Windows */
#ifdef WIN32
typedef unsigned char           UChar;
typedef __int16                 Int16;
typedef unsigned __int16        UInt16;
typedef __int32                 Int32;
typedef unsigned __int32        UInt32;
typedef __int64                 Int64;
typedef unsigned __int64        UInt64;

/* I386, 32-bit, non-Windows */
/* Sparc        */
/* MIPS         */
#elif __i386__ || __sun || __sgi
typedef unsigned char           UChar;
typedef short                   Int16;
typedef unsigned short          UInt16;
typedef long                    Int32;
typedef unsigned long           UInt32;
typedef long long               Int64;
typedef unsigned long long      UInt64;

/* 64-bit, non-Windows [silly hack by danbrown, separate for ease of changing values] */
#elif __x86_64__
typedef unsigned char           UChar;
typedef short                   Int16;
typedef unsigned short          UInt16;
typedef long                    Int32;
typedef unsigned long           UInt32;
typedef long long               Int64;
typedef unsigned long long      UInt64;

#else

/* yielding an error is preferable to yielding incorrect behavior */
#error "Please define the sized types for your platform in chm_lib.c"
#endif

/* GCC */
#ifdef __GNUC__
#define memcmp __builtin_memcmp
#define memcpy __builtin_memcpy
#define strlen __builtin_strlen

#elif defined(WIN32)
static int ffs(unsigned int val)
{
        int bit=1, idx=1;
        while (bit != 0  &&  (val & bit) == 0)
        {
                bit <<= 1;
                ++idx;
        }
        if (bit == 0)
                return 0;
        else
                return idx;
}

#endif

/* utilities for unmarshalling data */
static int _unmarshal_char_array(unsigned char **pData,
                                 unsigned long *pLenRemain,
                                 char *dest,
                                 int count)
{
    if (count <= 0  ||  (unsigned int)count > *pLenRemain)
        return 0;
    memcpy(dest, (*pData), count);
    *pData += count;
    *pLenRemain -= count;
    return 1;
}

static int _unmarshal_uchar_array(unsigned char **pData,
                                  unsigned long *pLenRemain,
                                  unsigned char *dest,
                                  int count)
{
        if (count <= 0  ||  (unsigned int)count > *pLenRemain)
        return 0;
    memcpy(dest, (*pData), count);
    *pData += count;
    *pLenRemain -= count;
    return 1;
}

static int _unmarshal_int16(unsigned char **pData,
                            unsigned long *pLenRemain,
                            Int16 *dest)
{
    if (2 > *pLenRemain)
        return 0;
    *dest = (*pData)[0] | (*pData)[1]<<8;
    *pData += 2;
    *pLenRemain -= 2;
    return 1;
}

static int _unmarshal_uint16(unsigned char **pData,
                             unsigned long *pLenRemain,
                             UInt16 *dest)
{
    if (2 > *pLenRemain)
        return 0;
    *dest = (*pData)[0] | (*pData)[1]<<8;
    *pData += 2;
    *pLenRemain -= 2;
    return 1;
}

static int _unmarshal_int32(unsigned char **pData,
                            unsigned long *pLenRemain,
                            Int32 *dest)
{
    if (4 > *pLenRemain)
        return 0;
    *dest = (*pData)[0] | (*pData)[1]<<8 | (*pData)[2]<<16 | (*pData)[3]<<24;
    *pData += 4;
    *pLenRemain -= 4;
    return 1;
}

static int _unmarshal_uint32(unsigned char **pData,
                             unsigned long *pLenRemain,
                             UInt32 *dest)
{
    if (4 > *pLenRemain)
        return 0;
    *dest = (*pData)[0] | (*pData)[1]<<8 | (*pData)[2]<<16 | (*pData)[3]<<24;
    *pData += 4;
    *pLenRemain -= 4;
    return 1;
}

static int _unmarshal_int64(unsigned char **pData,
                            unsigned long *pLenRemain,
                            Int64 *dest)
{
    Int64 temp;
    int i;
    if (8 > *pLenRemain)
        return 0;
    temp=0;
    for(i=8; i>0; i--)
    {
        temp <<= 8;
        temp |= (*pData)[i-1];
    }
    *dest = temp;
    *pData += 8;
    *pLenRemain -= 8;
    return 1;
}

static int _unmarshal_uint64(unsigned char **pData,
                             unsigned long *pLenRemain,
                             UInt64 *dest)
{
    UInt64 temp;
    int i;
    if (8 > *pLenRemain)
        return 0;
    temp=0;
    for(i=8; i>0; i--)
    {
        temp <<= 8;
        temp |= (*pData)[i-1];
    }
    *dest = temp;
    *pData += 8;
    *pLenRemain -= 8;
    return 1;
}

static int _unmarshal_uuid(unsigned char **pData,
                           unsigned long *pDataLen,
                           unsigned char *dest)
{
    return _unmarshal_uchar_array(pData, pDataLen, dest, 16);
}

/* names of sections essential to decompression */
static const char _CHMU_RESET_TABLE[] =
        "::DataSpace/Storage/MSCompressed/Transform/"
        "{7FC28940-9D31-11D0-9B27-00A0C91E9C7C}/"
        "InstanceData/ResetTable";
static const char _CHMU_LZXC_CONTROLDATA[] =
        "::DataSpace/Storage/MSCompressed/ControlData";
static const char _CHMU_CONTENT[] =
        "::DataSpace/Storage/MSCompressed/Content";
static const char _CHMU_SPANINFO[] =
        "::DataSpace/Storage/MSCompressed/SpanInfo";

/*
 * structures local to this module
 */

/* structure of ITSF headers */
#define _CHM_ITSF_V2_LEN (0x58)
#define _CHM_ITSF_V3_LEN (0x60)
struct chmItsfHeader
{
    char        signature[4];           /*  0 (ITSF) */
    Int32       version;                /*  4 */
    Int32       header_len;             /*  8 */
    Int32       unknown_000c;           /*  c */
    UInt32      last_modified;          /* 10 */
    UInt32      lang_id;                /* 14 */
    UChar       dir_uuid[16];           /* 18 */
    UChar       stream_uuid[16];        /* 28 */
    UInt64      unknown_offset;         /* 38 */
    UInt64      unknown_len;            /* 40 */
    UInt64      dir_offset;             /* 48 */
    UInt64      dir_len;                /* 50 */
    UInt64      data_offset;            /* 58 (Not present before V3) */
}; /* __attribute__ ((aligned (1))); */

static int _unmarshal_itsf_header(unsigned char **pData,
                                  unsigned long *pDataLen,
                                  struct chmItsfHeader *dest)
{
    /* we only know how to deal with the 0x58 and 0x60 byte structures */
    if (*pDataLen != _CHM_ITSF_V2_LEN  &&  *pDataLen != _CHM_ITSF_V3_LEN)
        return 0;

    /* unmarshal common fields */
    _unmarshal_char_array(pData, pDataLen,  dest->signature, 4);
    _unmarshal_int32     (pData, pDataLen, &dest->version);
    _unmarshal_int32     (pData, pDataLen, &dest->header_len);
    _unmarshal_int32     (pData, pDataLen, &dest->unknown_000c);
    _unmarshal_uint32    (pData, pDataLen, &dest->last_modified);
    _unmarshal_uint32    (pData, pDataLen, &dest->lang_id);
    _unmarshal_uuid      (pData, pDataLen,  dest->dir_uuid);
    _unmarshal_uuid      (pData, pDataLen,  dest->stream_uuid);
    _unmarshal_uint64    (pData, pDataLen, &dest->unknown_offset);
    _unmarshal_uint64    (pData, pDataLen, &dest->unknown_len);
    _unmarshal_uint64    (pData, pDataLen, &dest->dir_offset);
    _unmarshal_uint64    (pData, pDataLen, &dest->dir_len);

    /* error check the data */
    /* XXX: should also check UUIDs, probably, though with a version 3 file,
     * current MS tools do not seem to use them.
     */
    if (memcmp(dest->signature, "ITSF", 4) != 0)
        return 0;
    if (dest->version == 2)
    {
        if (dest->header_len < _CHM_ITSF_V2_LEN)
            return 0;
    }
    else if (dest->version == 3)
    {
        if (dest->header_len < _CHM_ITSF_V3_LEN)
            return 0;
    }
    else
        return 0;

    /* now, if we have a V3 structure, unmarshal the rest.
     * otherwise, compute it
     */
    if (dest->version == 3)
    {
        if (*pDataLen != 0)
            _unmarshal_uint64(pData, pDataLen, &dest->data_offset);
        else
            return 0;
    }
    else
        dest->data_offset = dest->dir_offset + dest->dir_len;

    return 1;
}

/* structure of ITSP headers */
#define _CHM_ITSP_V1_LEN (0x54)
struct chmItspHeader
{
    char        signature[4];           /*  0 (ITSP) */
    Int32       version;                /*  4 */
    Int32       header_len;             /*  8 */
    Int32       unknown_000c;           /*  c */
    UInt32      block_len;              /* 10 */
    Int32       blockidx_intvl;         /* 14 */
    Int32       index_depth;            /* 18 */
    Int32       index_root;             /* 1c */
    Int32       index_head;             /* 20 */
    Int32       unknown_0024;           /* 24 */
    UInt32      num_blocks;             /* 28 */
    Int32       unknown_002c;           /* 2c */
    UInt32      lang_id;                /* 30 */
    UChar       system_uuid[16];        /* 34 */
    UChar       unknown_0044[16];       /* 44 */
}; /* __attribute__ ((aligned (1))); */

static int _unmarshal_itsp_header(unsigned char **pData,
                                  unsigned long *pDataLen,
                                  struct chmItspHeader *dest)
{
    /* we only know how to deal with a 0x54 byte structures */
    if (*pDataLen != _CHM_ITSP_V1_LEN)
        return 0;

    /* unmarshal fields */
    _unmarshal_char_array(pData, pDataLen,  dest->signature, 4);
    _unmarshal_int32     (pData, pDataLen, &dest->version);
    _unmarshal_int32     (pData, pDataLen, &dest->header_len);
    _unmarshal_int32     (pData, pDataLen, &dest->unknown_000c);
    _unmarshal_uint32    (pData, pDataLen, &dest->block_len);
    _unmarshal_int32     (pData, pDataLen, &dest->blockidx_intvl);
    _unmarshal_int32     (pData, pDataLen, &dest->index_depth);
    _unmarshal_int32     (pData, pDataLen, &dest->index_root);
    _unmarshal_int32     (pData, pDataLen, &dest->index_head);
    _unmarshal_int32     (pData, pDataLen, &dest->unknown_0024);
    _unmarshal_uint32    (pData, pDataLen, &dest->num_blocks);
    _unmarshal_int32     (pData, pDataLen, &dest->unknown_002c);
    _unmarshal_uint32    (pData, pDataLen, &dest->lang_id);
    _unmarshal_uuid      (pData, pDataLen,  dest->system_uuid);
    _unmarshal_uchar_array(pData, pDataLen, dest->unknown_0044, 16);

    /* error check the data */
    if (memcmp(dest->signature, "ITSP", 4) != 0)
        return 0;
    if (dest->version != 1)
        return 0;
    if (dest->header_len != _CHM_ITSP_V1_LEN)
        return 0;

    return 1;
}

/* structure of PMGL headers */
static const char _chm_pmgl_marker[4] = "PMGL";
#define _CHM_PMGL_LEN (0x14)
struct chmPmglHeader
{
    char        signature[4];           /*  0 (PMGL) */
    UInt32      free_space;             /*  4 */
    UInt32      unknown_0008;           /*  8 */
    Int32       block_prev;             /*  c */
    Int32       block_next;             /* 10 */
}; /* __attribute__ ((aligned (1))); */

static int _unmarshal_pmgl_header(unsigned char **pData,
                                  unsigned long *pDataLen,
                                  struct chmPmglHeader *dest)
{
    /* we only know how to deal with a 0x14 byte structures */
    if (*pDataLen != _CHM_PMGL_LEN)
        return 0;

    /* unmarshal fields */
    _unmarshal_char_array(pData, pDataLen,  dest->signature, 4);
    _unmarshal_uint32    (pData, pDataLen, &dest->free_space);
    _unmarshal_uint32    (pData, pDataLen, &dest->unknown_0008);
    _unmarshal_int32     (pData, pDataLen, &dest->block_prev);
    _unmarshal_int32     (pData, pDataLen, &dest->block_next);

    /* check structure */
    if (memcmp(dest->signature, _chm_pmgl_marker, 4) != 0)
        return 0;

    return 1;
}

/* structure of PMGI headers */
static const char _chm_pmgi_marker[4] = "PMGI";
#define _CHM_PMGI_LEN (0x08)
struct chmPmgiHeader
{
    char        signature[4];           /*  0 (PMGI) */
    UInt32      free_space;             /*  4 */
}; /* __attribute__ ((aligned (1))); */

static int _unmarshal_pmgi_header(unsigned char **pData,
                                  unsigned long *pDataLen,
                                  struct chmPmgiHeader *dest)
{
    /* we only know how to deal with a 0x8 byte structures */
    if (*pDataLen != _CHM_PMGI_LEN)
        return 0;

    /* unmarshal fields */
    _unmarshal_char_array(pData, pDataLen,  dest->signature, 4);
    _unmarshal_uint32    (pData, pDataLen, &dest->free_space);

    /* check structure */
    if (memcmp(dest->signature, _chm_pmgi_marker, 4) != 0)
        return 0;

    return 1;
}

/* structure of LZXC reset table */
#define _CHM_LZXC_RESETTABLE_V1_LEN (0x28)
struct chmLzxcResetTable
{
    UInt32      version;
    UInt32      block_count;
    UInt32      unknown;
    UInt32      table_offset;
    UInt64      uncompressed_len;
    UInt64      compressed_len;
    UInt64      block_len;     
}; /* __attribute__ ((aligned (1))); */

static int _unmarshal_lzxc_reset_table(unsigned char **pData,
                                       unsigned long *pDataLen,
                                       struct chmLzxcResetTable *dest)
{
    /* we only know how to deal with a 0x28 byte structures */
    if (*pDataLen != _CHM_LZXC_RESETTABLE_V1_LEN)
        return 0;

    /* unmarshal fields */
    _unmarshal_uint32    (pData, pDataLen, &dest->version);
    _unmarshal_uint32    (pData, pDataLen, &dest->block_count);
    _unmarshal_uint32    (pData, pDataLen, &dest->unknown);
    _unmarshal_uint32    (pData, pDataLen, &dest->table_offset);
    _unmarshal_uint64    (pData, pDataLen, &dest->uncompressed_len);
    _unmarshal_uint64    (pData, pDataLen, &dest->compressed_len);
    _unmarshal_uint64    (pData, pDataLen, &dest->block_len);

    /* check structure */
    if (dest->version != 2)
        return 0;

    return 1;
}

/* structure of LZXC control data block */
#define _CHM_LZXC_MIN_LEN (0x18)
#define _CHM_LZXC_V2_LEN (0x1c)
struct chmLzxcControlData
{
    UInt32      size;                   /*  0        */
    char        signature[4];           /*  4 (LZXC) */
    UInt32      version;                /*  8        */
    UInt32      resetInterval;          /*  c        */
    UInt32      windowSize;             /* 10        */
    UInt32      unknown_14;             /* 14        */
    UInt32      unknown_18;             /* 18        */
};

static int _unmarshal_lzxc_control_data(unsigned char **pData,
                                        unsigned long *pDataLen,
                                        struct chmLzxcControlData *dest)
{
    /* we want at least 0x18 bytes */
    if (*pDataLen < _CHM_LZXC_MIN_LEN)
        return 0;

    /* unmarshal fields */
    _unmarshal_uint32    (pData, pDataLen, &dest->size);
    _unmarshal_char_array(pData, pDataLen,  dest->signature, 4);
    _unmarshal_uint32    (pData, pDataLen, &dest->version);
    _unmarshal_uint32    (pData, pDataLen, &dest->resetInterval);
    _unmarshal_uint32    (pData, pDataLen, &dest->windowSize);
    _unmarshal_uint32    (pData, pDataLen, &dest->unknown_14);

    if (*pDataLen >= _CHM_LZXC_V2_LEN)
        _unmarshal_uint32    (pData, pDataLen, &dest->unknown_18);
    else
        dest->unknown_18 = 0;

    if (dest->version == 2)
    {
        dest->resetInterval *= 0x8000;
        dest->windowSize *= 0x8000;
        dest->unknown_14 *= 0x8000;
    }
    if (dest->windowSize == 0  ||  dest->resetInterval == 0)
        return 0;

    /* for now, only support resetInterval a multiple of windowSize/2 */
    if (dest->windowSize == 1)
        return 0;
    if ((dest->resetInterval % (dest->windowSize/2)) != 0)
        return 0;

    /* check structure */
    if (memcmp(dest->signature, "LZXC", 4) != 0)
        return 0;

    return 1;
}

/* the structure used for chm file handles */
struct chmFile
{
#ifdef WIN32
    HANDLE              fd;
#else
    int                 fd;
#endif

#ifdef CHM_MT
#ifdef WIN32
    CRITICAL_SECTION    mutex;
    CRITICAL_SECTION    lzx_mutex;
    CRITICAL_SECTION    cache_mutex;
#else
    pthread_mutex_t     mutex;
    pthread_mutex_t     lzx_mutex;
    pthread_mutex_t     cache_mutex;
#endif
#endif

    UInt64              dir_offset;
    UInt64              dir_len;    
    UInt64              data_offset;
    Int32               index_root;
    Int32               index_head;
    UInt32              block_len;     

    UInt64              span;
    struct chmUnitInfo  rt_unit;
    struct chmUnitInfo  cn_unit;
    struct chmLzxcResetTable reset_table;

    /* LZX control data */
    UInt32              window_size;
    UInt32              reset_interval;
    UInt32              reset_blkcount;

    /* decompressor state */
    struct LZXstate    *lzx_state;
    int                 lzx_last_block;

    /* cache for decompressed blocks */
    UChar             **cache_blocks;
    Int64              *cache_block_indices;
    Int32               cache_num_blocks;
};

/*
 * utility functions local to this module
 */

/* utility function to handle differences between {pread,read}(64)? */
static Int64 _chm_fetch_bytes(struct chmFile *h,
                              UChar *buf,
                              UInt64 os,
                              Int64 len)
{
    Int64 readLen=0, oldOs=0;
    if (h->fd  ==  CHM_NULL_FD)
        return readLen;

    CHM_ACQUIRE_LOCK(h->mutex);
#ifdef CHM_USE_WIN32IO
    /* NOTE: this might be better done with CreateFileMapping, et cetera... */
    {
        DWORD origOffsetLo=0, origOffsetHi=0;
        DWORD offsetLo, offsetHi;
        DWORD actualLen=0;

        /* awkward Win32 Seek/Tell */
        offsetLo = (unsigned long)(os & 0xffffffffL);
        offsetHi = (unsigned long)((os >> 32) & 0xffffffffL);
        origOffsetLo = SetFilePointer(h->fd, 0, &origOffsetHi, FILE_CURRENT);
        offsetLo = SetFilePointer(h->fd, offsetLo, &offsetHi, FILE_BEGIN);

        /* read the data */
        if (ReadFile(h->fd,
                     buf,
                     (DWORD)len,
                     &actualLen,
                     NULL) == TRUE)
            readLen = actualLen;
        else
            readLen = 0;

        /* restore original position */
        SetFilePointer(h->fd, origOffsetLo, &origOffsetHi, FILE_BEGIN);
    }
#else
#ifdef CHM_USE_PREAD
#ifdef CHM_USE_IO64
    readLen = pread64(h->fd, buf, (long)len, os);
#else
    readLen = pread(h->fd, buf, (long)len, (unsigned long)os);
#endif
#else
#ifdef CHM_USE_IO64
    oldOs = lseek64(h->fd, 0, SEEK_CUR);
    lseek64(h->fd, os, SEEK_SET);
    readLen = read(h->fd, buf, len);
    lseek64(h->fd, oldOs, SEEK_SET);
#else
    oldOs = lseek(h->fd, 0, SEEK_CUR);
    lseek(h->fd, (long)os, SEEK_SET);
    readLen = read(h->fd, buf, len);
    lseek(h->fd, (long)oldOs, SEEK_SET);
#endif
#endif
#endif
    CHM_RELEASE_LOCK(h->mutex);
    return readLen;
}

/* open an ITS archive */
struct chmFile *chm_open(const char *filename)
{
    unsigned char               sbuffer[256];
    unsigned long               sremain;
    unsigned char              *sbufpos;
    struct chmFile             *newHandle=NULL;
    struct chmItsfHeader        itsfHeader;
    struct chmItspHeader        itspHeader;
    struct chmUnitInfo          uiSpan;
    struct chmUnitInfo          uiLzxc;
    struct chmLzxcControlData   ctlData;

    /* allocate handle */
    newHandle = (struct chmFile *)malloc(sizeof(struct chmFile));
    newHandle->fd = CHM_NULL_FD;
    newHandle->lzx_state = NULL;
    newHandle->cache_blocks = NULL;
    newHandle->cache_block_indices = NULL;
    newHandle->cache_num_blocks = 0;

    /* open file */
#ifdef WIN32
    if ((newHandle->fd=CreateFileA(filename,
                                   GENERIC_READ,
                                   0,
                                   NULL,
                                   OPEN_EXISTING,
                                   FILE_ATTRIBUTE_NORMAL,
                                   NULL)) == CHM_NULL_FD)
    {
        free(newHandle);
        return NULL;
    }
#else
    if ((newHandle->fd=open(filename, O_RDONLY)) == CHM_NULL_FD)
    {
        free(newHandle);
        return NULL;
    }
#endif

    /* initialize mutexes, if needed */
#ifdef CHM_MT
#ifdef WIN32
    InitializeCriticalSection(&newHandle->mutex);
    InitializeCriticalSection(&newHandle->lzx_mutex);
    InitializeCriticalSection(&newHandle->cache_mutex);
#else
    pthread_mutex_init(&newHandle->mutex, NULL);
    pthread_mutex_init(&newHandle->lzx_mutex, NULL);
    pthread_mutex_init(&newHandle->cache_mutex, NULL);
#endif
#endif

    /* read and verify header */
    sremain = _CHM_ITSF_V3_LEN;
    sbufpos = sbuffer;
    if (_chm_fetch_bytes(newHandle, sbuffer, (UInt64)0, sremain) != sremain    ||
        !_unmarshal_itsf_header(&sbufpos, &sremain, &itsfHeader))
    {
        chm_close(newHandle);
        return NULL;
    }

    /* stash important values from header */
    newHandle->dir_offset  = itsfHeader.dir_offset;
    newHandle->dir_len     = itsfHeader.dir_len;
    newHandle->data_offset = itsfHeader.data_offset;

    /* now, read and verify the directory header chunk */
    sremain = _CHM_ITSP_V1_LEN;
    sbufpos = sbuffer;
    if (_chm_fetch_bytes(newHandle, sbuffer,
                         (UInt64)itsfHeader.dir_offset, sremain) != sremain       ||
        !_unmarshal_itsp_header(&sbufpos, &sremain, &itspHeader))
    {
        chm_close(newHandle);
        return NULL;
    }

    /* grab essential information from ITSP header */
    newHandle->dir_offset += itspHeader.header_len;
    newHandle->dir_len    -= itspHeader.header_len;
    newHandle->index_root  = itspHeader.index_root;
    newHandle->index_head  = itspHeader.index_head;
    newHandle->block_len   = itspHeader.block_len;

    /* if the index root is -1, this means we don't have any PMGI blocks.
     * as a result, we must use the sole PMGL block as the index root
     */
    if (newHandle->index_root == -1)
        newHandle->index_root = newHandle->index_head;

    /* prefetch most commonly needed unit infos */
    if (CHM_RESOLVE_SUCCESS != chm_resolve_object(newHandle,
                                                  _CHMU_SPANINFO,
                                                  &uiSpan)                ||
        uiSpan.space == CHM_COMPRESSED                                    ||
        CHM_RESOLVE_SUCCESS != chm_resolve_object(newHandle,
                                                  _CHMU_RESET_TABLE,
                                                  &newHandle->rt_unit)    ||
        newHandle->rt_unit.space == CHM_COMPRESSED                        ||
        CHM_RESOLVE_SUCCESS != chm_resolve_object(newHandle,
                                                  _CHMU_CONTENT,
                                                  &newHandle->cn_unit)    ||
        newHandle->cn_unit.space == CHM_COMPRESSED                        ||
        CHM_RESOLVE_SUCCESS != chm_resolve_object(newHandle,
                                                  _CHMU_LZXC_CONTROLDATA,
                                                  &uiLzxc)                ||
        uiLzxc.space == CHM_COMPRESSED)
    {
        chm_close(newHandle);
        return NULL;
    }

    /* try to read span */
    /* N.B.: we've already checked that uiSpan is in the uncompressed section,
     *       so this should not require attempting to decompress, which may
     *       rely on having a valid "span"
     */
    sremain = 8;
    sbufpos = sbuffer;
    if (chm_retrieve_object(newHandle, &uiSpan, sbuffer,
                            0, sremain) != sremain                        ||
        !_unmarshal_uint64(&sbufpos, &sremain, &newHandle->span))
    {
        chm_close(newHandle);
        return NULL;
    }

    /* read reset table info */
    sremain = _CHM_LZXC_RESETTABLE_V1_LEN;
    sbufpos = sbuffer;
    if (chm_retrieve_object(newHandle, &newHandle->rt_unit, sbuffer,
                            0, sremain) != sremain                        ||
        !_unmarshal_lzxc_reset_table(&sbufpos, &sremain,
                                     &newHandle->reset_table))
    {
        chm_close(newHandle);
        return NULL;
    }

    /* read control data */
    sremain = (unsigned long)uiLzxc.length;
    sbufpos = sbuffer;
    if (chm_retrieve_object(newHandle, &uiLzxc, sbuffer,
                             0, sremain) != sremain                       ||
        !_unmarshal_lzxc_control_data(&sbufpos, &sremain,
                                      &ctlData))
    {
        chm_close(newHandle);
        return NULL;
    }
    newHandle->window_size = ctlData.windowSize;
    newHandle->reset_interval = ctlData.resetInterval;
    newHandle->reset_blkcount = newHandle->reset_interval /
                               (newHandle->window_size / 2);

    /* initialize cache */
    chm_set_param(newHandle, CHM_PARAM_MAX_BLOCKS_CACHED,
                  CHM_MAX_BLOCKS_CACHED);

    return newHandle;
}

/* close an ITS archive */
void chm_close(struct chmFile *h)
{
    if (h != NULL)
    {
        if (h->fd != CHM_NULL_FD)
            CHM_CLOSE_FILE(h->fd);
        h->fd = CHM_NULL_FD;

#ifdef CHM_MT
#ifdef WIN32
        DeleteCriticalSection(&h->mutex);
        DeleteCriticalSection(&h->lzx_mutex);
        DeleteCriticalSection(&h->cache_mutex);
#else
        pthread_mutex_destroy(&h->mutex);
        pthread_mutex_destroy(&h->lzx_mutex);
        pthread_mutex_destroy(&h->cache_mutex);
#endif
#endif

        if (h->lzx_state)
            LZXteardown(h->lzx_state);
        h->lzx_state = NULL;

        if (h->cache_blocks)
        {
            int i;
            for (i=0; i<h->cache_num_blocks; i++)
            {
                if (h->cache_blocks[i])
                    free(h->cache_blocks[i]);
            }
            free(h->cache_blocks);
            h->cache_blocks = NULL;
        }

        if (h->cache_block_indices)
            free(h->cache_block_indices);
        h->cache_block_indices = NULL;

        free(h);
    }
}

/*
 * set a parameter on the file handle.
 * valid parameter types:
 *          CHM_PARAM_MAX_BLOCKS_CACHED:
 *                 how many decompressed blocks should be cached?  A simple
 *                 caching scheme is used, wherein the index of the block is
 *                 used as a hash value, and hash collision results in the
 *                 invalidation of the previously cached block.
 */
void chm_set_param(struct chmFile *h,
                   int paramType,
                   int paramVal)
{
    switch (paramType)
    {
        case CHM_PARAM_MAX_BLOCKS_CACHED:
            CHM_ACQUIRE_LOCK(h->cache_mutex);
            if (paramVal != h->cache_num_blocks)
            {
                UChar **newBlocks;
                UInt64 *newIndices;
                int     i;

                /* allocate new cached blocks */
                newBlocks = (UChar **)malloc(paramVal * sizeof (UChar *));
                newIndices = (UInt64 *)malloc(paramVal * sizeof (UInt64));
                for (i=0; i<paramVal; i++)
                {
                    newBlocks[i] = NULL;
                    newIndices[i] = 0;
                }

                /* re-distribute old cached blocks */
                if (h->cache_blocks)
                {
                    for (i=0; i<h->cache_num_blocks; i++)
                    {
                        int newSlot = (int)(h->cache_block_indices[i] % paramVal);

                        if (h->cache_blocks[i])
                        {
                            /* in case of collision, destroy newcomer */
                            if (newBlocks[newSlot])
                            {
                                free(h->cache_blocks[i]);
                                h->cache_blocks[i] = NULL;
                            }
                            else
                            {
                                newBlocks[newSlot] = h->cache_blocks[i];
                                newIndices[newSlot] =
                                            h->cache_block_indices[i];
                            }
                        }
                    }

                    free(h->cache_blocks);
                    free(h->cache_block_indices);
                }

                /* now, set new values */
                h->cache_blocks = newBlocks;
                h->cache_block_indices = newIndices;
                h->cache_num_blocks = paramVal;
            }
            CHM_RELEASE_LOCK(h->cache_mutex);
            break;

        default:
            break;
    }
}

/*
 * helper methods for chm_resolve_object
 */

/* skip a compressed dword */
static void _chm_skip_cword(UChar **pEntry)
{
    while (*(*pEntry)++ >= 0x80)
        ;
}

/* skip the data from a PMGL entry */
static void _chm_skip_PMGL_entry_data(UChar **pEntry)
{
    _chm_skip_cword(pEntry);
    _chm_skip_cword(pEntry);
    _chm_skip_cword(pEntry);
}

/* parse a compressed dword */
static UInt64 _chm_parse_cword(UChar **pEntry)
{
    UInt64 accum = 0;
    UChar temp;
    while ((temp=*(*pEntry)++) >= 0x80)
    {
        accum <<= 7;
        accum += temp & 0x7f;
    }

    return (accum << 7) + temp;
}

/* parse a utf-8 string into an ASCII char buffer */
static int _chm_parse_UTF8(UChar **pEntry, UInt64 count, char *path)
{
    /* XXX: implement UTF-8 support, including a real mapping onto
     *      ISO-8859-1?  probably there is a library to do this?  As is
     *      immediately apparent from the below code, I'm only handling files
     *      in which none of the strings contain UTF-8 multi-byte characters.
     */
    while (count != 0)
    {
        if (*(*pEntry) > 0x7f)
            return 0;

        *path++ = (char)(*(*pEntry)++);
        --count;
    }

    *path = '\0';
    return 1;
}

/* parse a PMGL entry into a chmUnitInfo struct; return 1 on success. */
static int _chm_parse_PMGL_entry(UChar **pEntry, struct chmUnitInfo *ui)
{
    UInt64 strLen;

    /* parse str len */
    strLen = _chm_parse_cword(pEntry);
    if (strLen > CHM_MAX_PATHLEN)
        return 0;

    /* parse path */
    if (! _chm_parse_UTF8(pEntry, strLen, ui->path))
        return 0;

    /* parse info */
    ui->space  = (int)_chm_parse_cword(pEntry);
    ui->start  = _chm_parse_cword(pEntry);
    ui->length = _chm_parse_cword(pEntry);
    return 1;
}

/* find an exact entry in PMGL; return NULL if we fail */
static UChar *_chm_find_in_PMGL(UChar *page_buf,
                         UInt32 block_len,
                         const char *objPath)
{
    /* XXX: modify this to do a binary search using the nice index structure
     *      that is provided for us.
     */
    struct chmPmglHeader header;
    UInt32 hremain;
    UChar *end;
    UChar *cur;
    UChar *temp;
    UInt64 strLen;
    char buffer[CHM_MAX_PATHLEN+1];

    /* figure out where to start and end */
    cur = page_buf;
    hremain = _CHM_PMGL_LEN;
    if (! _unmarshal_pmgl_header(&cur, &hremain, &header))
        return NULL;
    end = page_buf + block_len - (header.free_space);

    /* now, scan progressively */
    while (cur < end)
    {
        /* grab the name */
        temp = cur;
        strLen = _chm_parse_cword(&cur);
        if (! _chm_parse_UTF8(&cur, strLen, buffer))
            return NULL;

        /* check if it is the right name */
#ifdef WIN32
                if (! stricmp(buffer, objPath))
                        return temp;
#else
        if (! strcasecmp(buffer, objPath))
            return temp;
#endif

        _chm_skip_PMGL_entry_data(&cur);
    }

    return NULL;
}

/* find which block should be searched next for the entry; -1 if no block */
static Int32 _chm_find_in_PMGI(UChar *page_buf,
                        UInt32 block_len,
                        const char *objPath)
{
    /* XXX: modify this to do a binary search using the nice index structure
     *      that is provided for us
     */
    struct chmPmgiHeader header;
    UInt32 hremain;
    int page=-1;
    UChar *end;
    UChar *cur;
    UInt64 strLen;
    char buffer[CHM_MAX_PATHLEN+1];

    /* figure out where to start and end */
    cur = page_buf;
    hremain = _CHM_PMGI_LEN;
    if (! _unmarshal_pmgi_header(&cur, &hremain, &header))
        return -1;
    end = page_buf + block_len - (header.free_space);

    /* now, scan progressively */
    while (cur < end)
    {
        /* grab the name */
        strLen = _chm_parse_cword(&cur);
        if (! _chm_parse_UTF8(&cur, strLen, buffer))
            return -1;

        /* check if it is the right name */
#ifdef WIN32
        if (stricmp(buffer, objPath) > 0)
            return page;
#else
        if (strcasecmp(buffer, objPath) > 0)
            return page;
#endif

        /* load next value for path */
        page = (int)_chm_parse_cword(&cur);
    }

    return page;
}

/* resolve a particular object from the archive */
int chm_resolve_object(struct chmFile *h,
                       const char *objPath,
                       struct chmUnitInfo *ui)
{
    /*
     * XXX: implement caching scheme for dir pages
     */

    Int32 curPage;

    /* buffer to hold whatever page we're looking at */
#ifdef WIN32
        UChar *page_buf = alloca(h->block_len);
#else
    UChar page_buf[h->block_len];
#endif

    /* starting page */
    curPage = h->index_root;

    /* until we have either returned or given up */
    while (curPage != -1)
    {

        /* try to fetch the index page */
        if (_chm_fetch_bytes(h, page_buf,
                             (UInt64)h->dir_offset + (UInt64)curPage*h->block_len,
                             h->block_len) != h->block_len)
            return CHM_RESOLVE_FAILURE;

        /* now, if it is a leaf node: */
        if (memcmp(page_buf, _chm_pmgl_marker, 4) == 0)
        {
            /* scan block */
            UChar *pEntry = _chm_find_in_PMGL(page_buf,
                                              h->block_len,
                                              objPath);
            if (pEntry == NULL)
                return CHM_RESOLVE_FAILURE;

            /* parse entry and return */
            _chm_parse_PMGL_entry(&pEntry, ui);
            return CHM_RESOLVE_SUCCESS;
        }

        /* else, if it is a branch node: */
        else if (memcmp(page_buf, _chm_pmgi_marker, 4) == 0)
            curPage = _chm_find_in_PMGI(page_buf, h->block_len, objPath);

        /* else, we are confused.  give up. */
        else
            return CHM_RESOLVE_FAILURE;
    }

        /* didn't find anything.  fail. */
        return CHM_RESOLVE_FAILURE;
}

/*
 * utility methods for dealing with compressed data
 */

/* get the bounds of a compressed block.  return 0 on failure */
static int _chm_get_cmpblock_bounds(struct chmFile *h,
                             UInt64 block,
                             UInt64 *start,
                             Int64 *len)
{
    UChar buffer[8], *dummy;
    UInt32 remain;

    /* for all but the last block, use the reset table */
    if (block < h->reset_table.block_count-1)
    {
        /* unpack the start address */
        dummy = buffer;
        remain = 8;
        if (_chm_fetch_bytes(h, buffer,
                             (UInt64)h->data_offset
                                + (UInt64)h->rt_unit.start
                                + (UInt64)h->reset_table.table_offset
                                + (UInt64)block*8,
                             remain) != remain                            ||
            !_unmarshal_uint64(&dummy, &remain, start))
            return 0;

        /* unpack the end address */
        dummy = buffer;
        remain = 8;
        if (_chm_fetch_bytes(h, buffer,
                         (UInt64)h->data_offset
                                + (UInt64)h->rt_unit.start
                                + (UInt64)h->reset_table.table_offset
                                + (UInt64)block*8 + 8,
                         remain) != remain                                ||
            !_unmarshal_int64(&dummy, &remain, len))
            return 0;
    }

    /* for the last block, use the span in addition to the reset table */
    else
    {
        /* unpack the start address */
        dummy = buffer;
        remain = 8;
        if (_chm_fetch_bytes(h, buffer,
                             (UInt64)h->data_offset
                                + (UInt64)h->rt_unit.start
                                + (UInt64)h->reset_table.table_offset
                                + (UInt64)block*8,
                             remain) != remain                            ||
            !_unmarshal_uint64(&dummy, &remain, start))
            return 0;

        *len = h->reset_table.compressed_len;
    }

    /* compute the length and absolute start address */
    *len -= *start;
    *start += h->data_offset + h->cn_unit.start;

    return 1;
}

/* decompress the block.  must have lzx_mutex. */
static Int64 _chm_decompress_block(struct chmFile *h,
                                   UInt64 block,
                                   UChar **ubuffer)
{
#ifdef WIN32
        UChar *cbuffer = alloca(((unsigned int)h->reset_table.block_len + 6144));
#else
    UChar cbuffer[h->reset_table.block_len + 6144];     /* compressed buffer */
#endif
    UInt64 cmpStart;                                    /* compressed start  */
    Int64 cmpLen;                                       /* compressed len    */
    int indexSlot;                                      /* cache index slot  */
    UChar *lbuffer;                                     /* local buffer ptr  */
    UInt32 blockAlign = (UInt32)(block % h->reset_blkcount); /* reset intvl. aln. */
    UInt32 i;                                           /* local loop index  */

    /* check if we need previous blocks */
    if (blockAlign != 0)
    {
        /* fetch all required previous blocks since last reset */
        for (i = h->reset_blkcount - blockAlign; i > 0; i--)
        {

            /* check if we most recently decompressed the previous block */
            if (h->lzx_last_block != block-i)
            {
                indexSlot = (int)((block-i) % h->cache_num_blocks);
                h->cache_block_indices[indexSlot] = block-i;
                if (! h->cache_blocks[indexSlot])
                    h->cache_blocks[indexSlot] = (UChar *)malloc(
                                            (unsigned int)(h->reset_table.block_len));
                lbuffer = h->cache_blocks[indexSlot];

                /* decompress the previous block */
                LZXreset(h->lzx_state);
                if (!_chm_get_cmpblock_bounds(h, block-i, &cmpStart, &cmpLen) ||
                    _chm_fetch_bytes(h, cbuffer, cmpStart, cmpLen) != cmpLen  ||
                    LZXdecompress(h->lzx_state, cbuffer, lbuffer, (int)cmpLen,
                                  (int)h->reset_table.block_len) != DECR_OK)
                    return (Int64)0;
            }

            h->lzx_last_block = (int)(block - i);
        }
    }
    else
        LZXreset(h->lzx_state);

    /* allocate slot in cache */
    indexSlot = (int)(block % h->cache_num_blocks);
    h->cache_block_indices[indexSlot] = block;
    if (! h->cache_blocks[indexSlot])
        h->cache_blocks[indexSlot] = (UChar *)malloc(
                                          ((unsigned int)h->reset_table.block_len));
    lbuffer = h->cache_blocks[indexSlot];
    *ubuffer = lbuffer;

    /* decompress the block we actually want */
    if (! _chm_get_cmpblock_bounds(h, block, &cmpStart, &cmpLen)          ||
        _chm_fetch_bytes(h, cbuffer, cmpStart, cmpLen) != cmpLen          ||
        LZXdecompress(h->lzx_state, cbuffer, lbuffer, (int)cmpLen,
                      (int)h->reset_table.block_len) != DECR_OK)
        return (Int64)0;
    h->lzx_last_block = (int)block;

    /* XXX: modify LZX routines to return the length of the data they
     * decompressed and return that instead, for an extra sanity check.
     */
    return h->reset_table.block_len;
}

/* grab a region from a compressed block */
static Int64 _chm_decompress_region(struct chmFile *h,
                                    UChar *buf,
                                    UInt64 start,
                                    Int64 len)
{
    UInt64 nBlock, nOffset;
    UInt64 nLen;
    UInt64 gotLen;
    UChar *ubuffer;

        if (len <= 0)
                return (Int64)0;

    /* figure out what we need to read */
    nBlock = start / h->reset_table.block_len;
    nOffset = start % h->reset_table.block_len;
    nLen = len;
    if (nLen > (h->reset_table.block_len - nOffset))
        nLen = h->reset_table.block_len - nOffset;

    /* if block is cached, return data from it. */
    CHM_ACQUIRE_LOCK(h->lzx_mutex);
    CHM_ACQUIRE_LOCK(h->cache_mutex);
    if (h->cache_block_indices[nBlock % h->cache_num_blocks] == nBlock    &&
        h->cache_blocks[nBlock % h->cache_num_blocks] != NULL)
    {
        memcpy(buf,
               h->cache_blocks[nBlock % h->cache_num_blocks] + nOffset,
               (unsigned int)nLen);
        CHM_RELEASE_LOCK(h->cache_mutex);
        CHM_RELEASE_LOCK(h->lzx_mutex);
        return nLen;
    }
    CHM_RELEASE_LOCK(h->cache_mutex);

    /* data request not satisfied, so... start up the decompressor machine */
    if (! h->lzx_state)
    {
        int window_size = ffs(h->window_size) - 1;
        h->lzx_last_block = -1;
        h->lzx_state = LZXinit(window_size);
    }

    /* decompress some data */
    gotLen = _chm_decompress_block(h, nBlock, &ubuffer);
    if (gotLen < nLen)
        nLen = gotLen;
    memcpy(buf, ubuffer+nOffset, (unsigned int)nLen);
    CHM_RELEASE_LOCK(h->lzx_mutex);
    return nLen;
}

/* retrieve (part of) an object */
LONGINT64 chm_retrieve_object(struct chmFile *h,
                               struct chmUnitInfo *ui,
                               unsigned char *buf,
                               LONGUINT64 addr,
                               LONGINT64 len)
{
    /* must be valid file handle */
    if (h == NULL)
        return (Int64)0;

    /* starting address must be in correct range */
    if (addr < 0  ||  addr >= ui->length)
        return (Int64)0;

    /* clip length */
    if (addr + len > ui->length)
        len = ui->length - addr;

    /* if the file is uncompressed, it's simple */
    if (ui->space == CHM_UNCOMPRESSED)
    {
        /* read data */
        return _chm_fetch_bytes(h,
                                buf,
                                (UInt64)h->data_offset + (UInt64)ui->start + (UInt64)addr,
                                len);
    }

    /* else if the file is compressed, it's a little trickier */
    else /* ui->space == CHM_COMPRESSED */
    {
        Int64 swath=0, total=0;
        do {

            /* swill another mouthful */
            swath = _chm_decompress_region(h, buf, ui->start + addr, len);

            /* if we didn't get any... */
            if (swath == 0)
                return total;

            /* update stats */
            total += swath;
            len -= swath;
            addr += swath;
            buf += swath;

        } while (len != 0);

        return total;
    }
}

/* enumerate the objects in the .chm archive */
int chm_enumerate(struct chmFile *h,
                  int what,
                  CHM_ENUMERATOR e,
                  void *context)
{
    Int32 curPage;

    /* buffer to hold whatever page we're looking at */
#ifdef WIN32
        UChar *page_buf = alloca((unsigned int)h->block_len);
#else
    UChar page_buf[h->block_len];
#endif
    struct chmPmglHeader header;
    UChar *end;
    UChar *cur;
    unsigned long lenRemain;

    /* the current ui */
    struct chmUnitInfo ui;
    int flag;

    /* starting page */
    curPage = h->index_head;

    /* until we have either returned or given up */
    while (curPage != -1)
    {

        /* try to fetch the index page */
        if (_chm_fetch_bytes(h,
                             page_buf,
                             (UInt64)h->dir_offset + (UInt64)curPage*h->block_len,
                             h->block_len) != h->block_len)
            return 0;

        /* figure out start and end for this page */
        cur = page_buf;
        lenRemain = _CHM_PMGL_LEN;
        if (! _unmarshal_pmgl_header(&cur, &lenRemain, &header))
            return 0;
        end = page_buf + h->block_len - (header.free_space);

        /* loop over this page */
        while (cur < end)
        {
            if (! _chm_parse_PMGL_entry(&cur, &ui))
                return 0;

            /* check for DIRS */
            if (ui.length == 0  &&  !(what & CHM_ENUMERATE_DIRS))
                continue;

            /* check for FILES */
            if (ui.length != 0  &&  !(what & CHM_ENUMERATE_FILES))
                continue;

            /* check for NORMAL vs. META */
            if (ui.path[0] == '/')
            {

                /* check for NORMAL vs. SPECIAL */
                if (ui.path[1] == '#'  ||  ui.path[1] == '$')
                    flag = CHM_ENUMERATE_SPECIAL;
                else
                    flag = CHM_ENUMERATE_NORMAL;
            }
            else
                flag = CHM_ENUMERATE_META;
            if (! (what & flag))
                continue;

            /* call the enumerator */
            {
                int status = (*e)(h, &ui, context);
                switch (status)
                {
                    case CHM_ENUMERATOR_FAILURE:  return 0;
                    case CHM_ENUMERATOR_CONTINUE: break;
                    case CHM_ENUMERATOR_SUCCESS:  return 1;
                    default:                      break;
                }
            }
        }

        /* advance to next page */
        curPage = header.block_next;
    }

    return 1;
}

int chm_enumerate_dir(struct chmFile *h,
                      const char *prefix,
                      int what,
                      CHM_ENUMERATOR e,
                      void *context)
{
    /*
     * XXX: do this efficiently (i.e. using the tree index)
     */

    Int32 curPage;

    /* buffer to hold whatever page we're looking at */
#ifdef WIN32
        UChar *page_buf = alloca((unsigned int)h->block_len);
#else
    UChar page_buf[h->block_len];
#endif
    struct chmPmglHeader header;
    UChar *end;
    UChar *cur;
    unsigned long lenRemain;

    /* set to 1 once we've started */
    int it_has_begun=0;

    /* the current ui */
    struct chmUnitInfo ui;
    int flag;

    /* the length of the prefix */
    char prefixRectified[CHM_MAX_PATHLEN+1];
    int prefixLen;
    char lastPath[CHM_MAX_PATHLEN];
    int lastPathLen;

    /* starting page */
    curPage = h->index_head;

    /* initialize pathname state */
    strncpy(prefixRectified, prefix, CHM_MAX_PATHLEN);
    prefixLen = strlen(prefixRectified);
    if (prefixLen != 0)
    {
        if (prefixRectified[prefixLen-1] != '/')
        {
            prefixRectified[prefixLen] = '/';
            prefixRectified[prefixLen+1] = '\0';
            ++prefixLen;
        }
    }
    lastPath[0] = '\0';
    lastPathLen = -1;

    /* until we have either returned or given up */
    while (curPage != -1)
    {

        /* try to fetch the index page */
        if (_chm_fetch_bytes(h,
                             page_buf,
                             (UInt64)h->dir_offset + (UInt64)curPage*h->block_len,
                             h->block_len) != h->block_len)
            return 0;

        /* figure out start and end for this page */
        cur = page_buf;
        lenRemain = _CHM_PMGL_LEN;
        if (! _unmarshal_pmgl_header(&cur, &lenRemain, &header))
            return 0;
        end = page_buf + h->block_len - (header.free_space);

        /* loop over this page */
        while (cur < end)
        {
            if (! _chm_parse_PMGL_entry(&cur, &ui))
                return 0;

            /* check if we should start */
            if (! it_has_begun)
            {
                if (ui.length == 0  &&  strncmp(ui.path, prefixRectified, prefixLen) == 0)
                    it_has_begun = 1;
                else
                    continue;

                if (ui.path[prefixLen] == '\0')
                    continue;
            }

            /* check if we should stop */
            else
            {
                if (strncmp(ui.path, prefixRectified, prefixLen) != 0)
                    return 1;
            }

            /* check if we should include this path */
            if (lastPathLen != -1)
            {
                if (strncmp(ui.path, lastPath, lastPathLen) == 0)
                    continue;
            }
            strcpy(lastPath, ui.path);
            lastPathLen = strlen(lastPath);

            /* check for DIRS */
            if (ui.length == 0  &&  !(what & CHM_ENUMERATE_DIRS))
                continue;

            /* check for FILES */
            if (ui.length != 0  &&  !(what & CHM_ENUMERATE_FILES))
                continue;

            /* check for NORMAL vs. META */
            if (ui.path[0] == '/')
            {

                /* check for NORMAL vs. SPECIAL */
                if (ui.path[1] == '#'  ||  ui.path[1] == '$')
                    flag = CHM_ENUMERATE_SPECIAL;
                else
                    flag = CHM_ENUMERATE_NORMAL;
            }
            else
                flag = CHM_ENUMERATE_META;
            if (! (what & flag))
                continue;

            /* call the enumerator */
            {
                int status = (*e)(h, &ui, context);
                switch (status)
                {
                    case CHM_ENUMERATOR_FAILURE:  return 0;
                    case CHM_ENUMERATOR_CONTINUE: break;
                    case CHM_ENUMERATOR_SUCCESS:  return 1;
                    default:                      break;
                }
            }
        }

        /* advance to next page */
        curPage = header.block_next;
    }

    return 1;
}

/* $Id$ */
/***************************************************************************
 *                        lzx.c - LZX decompression routines               *
 *                           -------------------                           *
 *                                                                         *
 *  maintainer: Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  source:     modified lzx.c from cabextract v0.5                        *
 *  notes:      This file was taken from cabextract v0.5, which was,       *
 *              itself, a modified version of the lzx decompression code   *
 *              from unlzx.                                                *
 *                                                                         *
 *  platforms:  In its current incarnation, this file has been tested on   *
 *              two different Linux platforms (one, redhat-based, with a   *
 *              2.1.2 glibc and gcc 2.95.x, and the other, Debian, with    *
 *              2.2.4 glibc and both gcc 2.95.4 and gcc 3.0.2).  Both were *
 *              Intel x86 compatible machines.                             *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.  Note that an exemption to this   *
 *   license has been granted by Stuart Caie for the purposes of           *
 *   distribution with chmlib.  This does not, to the best of my           *
 *   knowledge, constitute a change in the license of this (the LZX) code  *
 *   in general.                                                           *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#ifdef __GNUC__
#define memcpy __builtin_memcpy
#endif

/* sized types */
typedef unsigned char  UBYTE; /* 8 bits exactly    */
typedef unsigned short UWORD; /* 16 bits (or more) */
typedef unsigned int   ULONG; /* 32 bits (or more) */
typedef   signed int    LONG; /* 32 bits (or more) */

/* some constants defined by the LZX specification */
#define LZX_MIN_MATCH                (2)
#define LZX_MAX_MATCH                (257)
#define LZX_NUM_CHARS                (256)
#define LZX_BLOCKTYPE_INVALID        (0)   /* also blocktypes 4-7 invalid */
#define LZX_BLOCKTYPE_VERBATIM       (1)
#define LZX_BLOCKTYPE_ALIGNED        (2)
#define LZX_BLOCKTYPE_UNCOMPRESSED   (3)
#define LZX_PRETREE_NUM_ELEMENTS     (20)
#define LZX_ALIGNED_NUM_ELEMENTS     (8)   /* aligned offset tree #elements */
#define LZX_NUM_PRIMARY_LENGTHS      (7)   /* this one missing from spec! */
#define LZX_NUM_SECONDARY_LENGTHS    (249) /* length tree #elements */

/* LZX huffman defines: tweak tablebits as desired */
#define LZX_PRETREE_MAXSYMBOLS  (LZX_PRETREE_NUM_ELEMENTS)
#define LZX_PRETREE_TABLEBITS   (6)
#define LZX_MAINTREE_MAXSYMBOLS (LZX_NUM_CHARS + 50*8)
#define LZX_MAINTREE_TABLEBITS  (12)
#define LZX_LENGTH_MAXSYMBOLS   (LZX_NUM_SECONDARY_LENGTHS+1)
#define LZX_LENGTH_TABLEBITS    (12)
#define LZX_ALIGNED_MAXSYMBOLS  (LZX_ALIGNED_NUM_ELEMENTS)
#define LZX_ALIGNED_TABLEBITS   (7)

#define LZX_LENTABLE_SAFETY (64) /* we allow length table decoding overruns */

#define LZX_DECLARE_TABLE(tbl) \
  UWORD tbl##_table[(1<<LZX_##tbl##_TABLEBITS) + (LZX_##tbl##_MAXSYMBOLS<<1)];\
  UBYTE tbl##_len  [LZX_##tbl##_MAXSYMBOLS + LZX_LENTABLE_SAFETY]

struct LZXstate
{
    UBYTE *window;         /* the actual decoding window              */
    ULONG window_size;     /* window size (32Kb through 2Mb)          */
    ULONG actual_size;     /* window size when it was first allocated */
    ULONG window_posn;     /* current offset within the window        */
    ULONG R0, R1, R2;      /* for the LRU offset system               */
    UWORD main_elements;   /* number of main tree elements            */
    int   header_read;     /* have we started decoding at all yet?    */
    UWORD block_type;      /* type of this block                      */
    ULONG block_length;    /* uncompressed length of this block       */
    ULONG block_remaining; /* uncompressed bytes still left to decode */
    ULONG frames_read;     /* the number of CFDATA blocks processed   */
    LONG  intel_filesize;  /* magic header value used for transform   */
    LONG  intel_curpos;    /* current offset in transform space       */
    int   intel_started;   /* have we seen any translatable data yet? */

    LZX_DECLARE_TABLE(PRETREE);
    LZX_DECLARE_TABLE(MAINTREE);
    LZX_DECLARE_TABLE(LENGTH);
    LZX_DECLARE_TABLE(ALIGNED);
};

/* LZX decruncher */

/* Microsoft's LZX document and their implementation of the
 * com.ms.util.cab Java package do not concur.
 *
 * In the LZX document, there is a table showing the correlation between
 * window size and the number of position slots. It states that the 1MB
 * window = 40 slots and the 2MB window = 42 slots. In the implementation,
 * 1MB = 42 slots, 2MB = 50 slots. The actual calculation is 'find the
 * first slot whose position base is equal to or more than the required
 * window size'. This would explain why other tables in the document refer
 * to 50 slots rather than 42.
 *
 * The constant NUM_PRIMARY_LENGTHS used in the decompression pseudocode
 * is not defined in the specification.
 *
 * The LZX document does not state the uncompressed block has an
 * uncompressed length field. Where does this length field come from, so
 * we can know how large the block is? The implementation has it as the 24
 * bits following after the 3 blocktype bits, before the alignment
 * padding.
 *
 * The LZX document states that aligned offset blocks have their aligned
 * offset huffman tree AFTER the main and length trees. The implementation
 * suggests that the aligned offset tree is BEFORE the main and length
 * trees.
 *
 * The LZX document decoding algorithm states that, in an aligned offset
 * block, if an extra_bits value is 1, 2 or 3, then that number of bits
 * should be read and the result added to the match offset. This is
 * correct for 1 and 2, but not 3, where just a huffman symbol (using the
 * aligned tree) should be read.
 *
 * Regarding the E8 preprocessing, the LZX document states 'No translation
 * may be performed on the last 6 bytes of the input block'. This is
 * correct.  However, the pseudocode provided checks for the *E8 leader*
 * up to the last 6 bytes. If the leader appears between -10 and -7 bytes
 * from the end, this would cause the next four bytes to be modified, at
 * least one of which would be in the last 6 bytes, which is not allowed
 * according to the spec.
 *
 * The specification states that the huffman trees must always contain at
 * least one element. However, many CAB files contain blocks where the
 * length tree is completely empty (because there are no matches), and
 * this is expected to succeed.
 */


/* LZX uses what it calls 'position slots' to represent match offsets.
 * What this means is that a small 'position slot' number and a small
 * offset from that slot are encoded instead of one large offset for
 * every match.
 * - position_base is an index to the position slot bases
 * - extra_bits states how many bits of offset-from-base data is needed.
 */
static const UBYTE extra_bits[51] = {
     0,  0,  0,  0,  1,  1,  2,  2,  3,  3,  4,  4,  5,  5,  6,  6,
     7,  7,  8,  8,  9,  9, 10, 10, 11, 11, 12, 12, 13, 13, 14, 14,
    15, 15, 16, 16, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17, 17,
    17, 17, 17
};

static const ULONG position_base[51] = {
          0,       1,       2,      3,      4,      6,      8,     12,     16,     24,     32,       48,      64,      96,     128,     192,
        256,     384,     512,    768,   1024,   1536,   2048,   3072,   4096,   6144,   8192,    12288,   16384,   24576,   32768,   49152,
      65536,   98304,  131072, 196608, 262144, 393216, 524288, 655360, 786432, 917504, 1048576, 1179648, 1310720, 1441792, 1572864, 1703936,
    1835008, 1966080, 2097152
};

struct LZXstate *LZXinit(int window)
{
    struct LZXstate *pState=NULL;
    ULONG wndsize = 1 << window;
    int i, posn_slots;

    /* LZX supports window sizes of 2^15 (32Kb) through 2^21 (2Mb) */
    /* if a previously allocated window is big enough, keep it     */
    if (window < 15 || window > 21) return NULL;

    /* allocate state and associated window */
    pState = (struct LZXstate *)malloc(sizeof(struct LZXstate));
    if (!(pState->window = (UBYTE *)malloc(wndsize)))
    {
        free(pState);
        return NULL;
    }
    pState->actual_size = wndsize;
    pState->window_size = wndsize;

    /* calculate required position slots */
    if (window == 20) posn_slots = 42;
    else if (window == 21) posn_slots = 50;
    else posn_slots = window << 1;

    /** alternatively **/
    /* posn_slots=i=0; while (i < wndsize) i += 1 << extra_bits[posn_slots++]; */

    /* initialize other state */
    pState->R0  =  pState->R1  = pState->R2 = 1;
    pState->main_elements   = LZX_NUM_CHARS + (posn_slots << 3);
    pState->header_read     = 0;
    pState->frames_read     = 0;
    pState->block_remaining = 0;
    pState->block_type      = LZX_BLOCKTYPE_INVALID;
    pState->intel_curpos    = 0;
    pState->intel_started   = 0;
    pState->window_posn     = 0;

    /* initialise tables to 0 (because deltas will be applied to them) */
    for (i = 0; i < LZX_MAINTREE_MAXSYMBOLS; i++) pState->MAINTREE_len[i] = 0;
    for (i = 0; i < LZX_LENGTH_MAXSYMBOLS; i++)   pState->LENGTH_len[i]   = 0;

    return pState;
}

void LZXteardown(struct LZXstate *pState)
{
    if (pState)
    {
        if (pState->window)
            free(pState->window);
        free(pState);
    }
}

int LZXreset(struct LZXstate *pState)
{
    int i;

    pState->R0  =  pState->R1  = pState->R2 = 1;
    pState->header_read     = 0;
    pState->frames_read     = 0;
    pState->block_remaining = 0;
    pState->block_type      = LZX_BLOCKTYPE_INVALID;
    pState->intel_curpos    = 0;
    pState->intel_started   = 0;
    pState->window_posn     = 0;

    for (i = 0; i < LZX_MAINTREE_MAXSYMBOLS + LZX_LENTABLE_SAFETY; i++) pState->MAINTREE_len[i] = 0;
    for (i = 0; i < LZX_LENGTH_MAXSYMBOLS + LZX_LENTABLE_SAFETY; i++)   pState->LENGTH_len[i]   = 0;

    return DECR_OK;
};


/* Bitstream reading macros:
 *
 * INIT_BITSTREAM    should be used first to set up the system
 * READ_BITS(var,n)  takes N bits from the buffer and puts them in var
 *
 * ENSURE_BITS(n)    ensures there are at least N bits in the bit buffer
 * PEEK_BITS(n)      extracts (without removing) N bits from the bit buffer
 * REMOVE_BITS(n)    removes N bits from the bit buffer
 *
 * These bit access routines work by using the area beyond the MSB and the
 * LSB as a free source of zeroes. This avoids having to mask any bits.
 * So we have to know the bit width of the bitbuffer variable. This is
 * sizeof(ULONG) * 8, also defined as ULONG_BITS
 */

/* number of bits in ULONG. Note: This must be at multiple of 16, and at
 * least 32 for the bitbuffer code to work (ie, it must be able to ensure
 * up to 17 bits - that's adding 16 bits when there's one bit left, or
 * adding 32 bits when there are no bits left. The code should work fine
 * for machines where ULONG >= 32 bits.
 */
#define ULONG_BITS (sizeof(ULONG)<<3)

#define INIT_BITSTREAM do { bitsleft = 0; bitbuf = 0; } while (0)

#define ENSURE_BITS(n)							\
  while (bitsleft < (n)) {						\
    bitbuf |= ((inpos[1]<<8)|inpos[0]) << (ULONG_BITS-16 - bitsleft);	\
    bitsleft += 16; inpos+=2;						\
  }

#define PEEK_BITS(n)   (bitbuf >> (ULONG_BITS - (n)))
#define REMOVE_BITS(n) ((bitbuf <<= (n)), (bitsleft -= (n)))

#define READ_BITS(v,n) do {						\
  ENSURE_BITS(n);							\
  (v) = PEEK_BITS(n);							\
  REMOVE_BITS(n);							\
} while (0)


/* Huffman macros */

#define TABLEBITS(tbl)   (LZX_##tbl##_TABLEBITS)
#define MAXSYMBOLS(tbl)  (LZX_##tbl##_MAXSYMBOLS)
#define SYMTABLE(tbl)    (pState->tbl##_table)
#define LENTABLE(tbl)    (pState->tbl##_len)

/* BUILD_TABLE(tablename) builds a huffman lookup table from code lengths.
 * In reality, it just calls make_decode_table() with the appropriate
 * values - they're all fixed by some #defines anyway, so there's no point
 * writing each call out in full by hand.
 */
#define BUILD_TABLE(tbl)						\
  if (make_decode_table(						\
    MAXSYMBOLS(tbl), TABLEBITS(tbl), LENTABLE(tbl), SYMTABLE(tbl)	\
  )) { return DECR_ILLEGALDATA; }


/* READ_HUFFSYM(tablename, var) decodes one huffman symbol from the
 * bitstream using the stated table and puts it in var.
 */
#define READ_HUFFSYM(tbl,var) do {					\
  ENSURE_BITS(16);							\
  hufftbl = SYMTABLE(tbl);						\
  if ((i = hufftbl[PEEK_BITS(TABLEBITS(tbl))]) >= MAXSYMBOLS(tbl)) {	\
    j = 1 << (ULONG_BITS - TABLEBITS(tbl));				\
    do {								\
      j >>= 1; i <<= 1; i |= (bitbuf & j) ? 1 : 0;			\
      if (!j) { return DECR_ILLEGALDATA; }	                        \
    } while ((i = hufftbl[i]) >= MAXSYMBOLS(tbl));			\
  }									\
  j = LENTABLE(tbl)[(var) = i];						\
  REMOVE_BITS(j);							\
} while (0)


/* READ_LENGTHS(tablename, first, last) reads in code lengths for symbols
 * first to last in the given table. The code lengths are stored in their
 * own special LZX way.
 */
#define READ_LENGTHS(tbl,first,last) do { \
  lb.bb = bitbuf; lb.bl = bitsleft; lb.ip = inpos; \
  if (lzx_read_lens(pState, LENTABLE(tbl),(first),(last),&lb)) { \
    return DECR_ILLEGALDATA; \
  } \
  bitbuf = lb.bb; bitsleft = lb.bl; inpos = lb.ip; \
} while (0)


/* make_decode_table(nsyms, nbits, length[], table[])
 *
 * This function was coded by David Tritscher. It builds a fast huffman
 * decoding table out of just a canonical huffman code lengths table.
 *
 * nsyms  = total number of symbols in this huffman tree.
 * nbits  = any symbols with a code length of nbits or less can be decoded
 *          in one lookup of the table.
 * length = A table to get code lengths from [0 to syms-1]
 * table  = The table to fill up with decoded symbols and pointers.
 *
 * Returns 0 for OK or 1 for error
 */

static int make_decode_table(ULONG nsyms, ULONG nbits, UBYTE *length, UWORD *table) {
    register UWORD sym;
    register ULONG leaf;
    register UBYTE bit_num = 1;
    ULONG fill;
    ULONG pos         = 0; /* the current position in the decode table */
    ULONG table_mask  = 1 << nbits;
    ULONG bit_mask    = table_mask >> 1; /* don't do 0 length codes */
    ULONG next_symbol = bit_mask; /* base of allocation for long codes */

    /* fill entries for codes short enough for a direct mapping */
    while (bit_num <= nbits) {
        for (sym = 0; sym < nsyms; sym++) {
            if (length[sym] == bit_num) {
                leaf = pos;

                if((pos += bit_mask) > table_mask) return 1; /* table overrun */

                /* fill all possible lookups of this symbol with the symbol itself */
                fill = bit_mask;
                while (fill-- > 0) table[leaf++] = sym;
            }
        }
        bit_mask >>= 1;
        bit_num++;
    }

    /* if there are any codes longer than nbits */
    if (pos != table_mask) {
        /* clear the remainder of the table */
        for (sym = pos; sym < table_mask; sym++) table[sym] = 0;

        /* give ourselves room for codes to grow by up to 16 more bits */
        pos <<= 16;
        table_mask <<= 16;
        bit_mask = 1 << 15;

        while (bit_num <= 16) {
            for (sym = 0; sym < nsyms; sym++) {
                if (length[sym] == bit_num) {
                    leaf = pos >> 16;
                    for (fill = 0; fill < bit_num - nbits; fill++) {
                        /* if this path hasn't been taken yet, 'allocate' two entries */
                        if (table[leaf] == 0) {
                            table[(next_symbol << 1)] = 0;
                            table[(next_symbol << 1) + 1] = 0;
                            table[leaf] = next_symbol++;
                        }
                        /* follow the path and select either left or right for next bit */
                        leaf = table[leaf] << 1;
                        if ((pos >> (15-fill)) & 1) leaf++;
                    }
                    table[leaf] = sym;

                    if ((pos += bit_mask) > table_mask) return 1; /* table overflow */
                }
            }
            bit_mask >>= 1;
            bit_num++;
        }
    }

    /* full table? */
    if (pos == table_mask) return 0;

    /* either erroneous table, or all elements are 0 - let's find out. */
    for (sym = 0; sym < nsyms; sym++) if (length[sym]) return 1;
    return 0;
}

struct lzx_bits {
  ULONG bb;
  int bl;
  UBYTE *ip;
};

static int lzx_read_lens(struct LZXstate *pState, UBYTE *lens, ULONG first, ULONG last, struct lzx_bits *lb) {
    ULONG i,j, x,y;
    int z;

    register ULONG bitbuf = lb->bb;
    register int bitsleft = lb->bl;
    UBYTE *inpos = lb->ip;
    UWORD *hufftbl;

    for (x = 0; x < 20; x++) {
        READ_BITS(y, 4);
        LENTABLE(PRETREE)[x] = y;
    }
    BUILD_TABLE(PRETREE);

    for (x = first; x < last; ) {
        READ_HUFFSYM(PRETREE, z);
        if (z == 17) {
            READ_BITS(y, 4); y += 4;
            while (y--) lens[x++] = 0;
        }
        else if (z == 18) {
            READ_BITS(y, 5); y += 20;
            while (y--) lens[x++] = 0;
        }
        else if (z == 19) {
            READ_BITS(y, 1); y += 4;
            READ_HUFFSYM(PRETREE, z);
            z = lens[x] - z; if (z < 0) z += 17;
            while (y--) lens[x++] = z;
        }
        else {
            z = lens[x] - z; if (z < 0) z += 17;
            lens[x++] = z;
        }
    }

    lb->bb = bitbuf;
    lb->bl = bitsleft;
    lb->ip = inpos;
    return 0;
}

int LZXdecompress(struct LZXstate *pState, unsigned char *inpos, unsigned char *outpos, int inlen, int outlen) {
    UBYTE *endinp = inpos + inlen;
    UBYTE *window = pState->window;
    UBYTE *runsrc, *rundest;
    UWORD *hufftbl; /* used in READ_HUFFSYM macro as chosen decoding table */

    ULONG window_posn = pState->window_posn;
    ULONG window_size = pState->window_size;
    ULONG R0 = pState->R0;
    ULONG R1 = pState->R1;
    ULONG R2 = pState->R2;

    register ULONG bitbuf;
    register int bitsleft;
    ULONG match_offset, i,j,k; /* ijk used in READ_HUFFSYM macro */
    struct lzx_bits lb; /* used in READ_LENGTHS macro */

    int togo = outlen, this_run, main_element, aligned_bits;
    int match_length, length_footer, extra, verbatim_bits;

    INIT_BITSTREAM;

    /* read header if necessary */
    if (!pState->header_read) {
        i = j = 0;
        READ_BITS(k, 1); if (k) { READ_BITS(i,16); READ_BITS(j,16); }
        pState->intel_filesize = (i << 16) | j; /* or 0 if not encoded */
        pState->header_read = 1;
    }

    /* main decoding loop */
    while (togo > 0) {
        /* last block finished, new block expected */
        if (pState->block_remaining == 0) {
            if (pState->block_type == LZX_BLOCKTYPE_UNCOMPRESSED) {
                if (pState->block_length & 1) inpos++; /* realign bitstream to word */
                INIT_BITSTREAM;
            }

            READ_BITS(pState->block_type, 3);
            READ_BITS(i, 16);
            READ_BITS(j, 8);
            pState->block_remaining = pState->block_length = (i << 8) | j;

            switch (pState->block_type) {
                case LZX_BLOCKTYPE_ALIGNED:
                    for (i = 0; i < 8; i++) { READ_BITS(j, 3); LENTABLE(ALIGNED)[i] = j; }
                    BUILD_TABLE(ALIGNED);
                    /* rest of aligned header is same as verbatim */

                case LZX_BLOCKTYPE_VERBATIM:
                    READ_LENGTHS(MAINTREE, 0, 256);
                    READ_LENGTHS(MAINTREE, 256, pState->main_elements);
                    BUILD_TABLE(MAINTREE);
                    if (LENTABLE(MAINTREE)[0xE8] != 0) pState->intel_started = 1;

                    READ_LENGTHS(LENGTH, 0, LZX_NUM_SECONDARY_LENGTHS);
                    BUILD_TABLE(LENGTH);
                    break;

                case LZX_BLOCKTYPE_UNCOMPRESSED:
                    pState->intel_started = 1; /* because we can't assume otherwise */
                    ENSURE_BITS(16); /* get up to 16 pad bits into the buffer */
                    if (bitsleft > 16) inpos -= 2; /* and align the bitstream! */
                    R0 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);inpos+=4;
                    R1 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);inpos+=4;
                    R2 = inpos[0]|(inpos[1]<<8)|(inpos[2]<<16)|(inpos[3]<<24);inpos+=4;
                    break;

                default:
                    return DECR_ILLEGALDATA;
            }
        }

        /* buffer exhaustion check */
        if (inpos > endinp) {
            /* it's possible to have a file where the next run is less than
             * 16 bits in size. In this case, the READ_HUFFSYM() macro used
             * in building the tables will exhaust the buffer, so we should
             * allow for this, but not allow those accidentally read bits to
             * be used (so we check that there are at least 16 bits
             * remaining - in this boundary case they aren't really part of
             * the compressed data)
             */
            if (inpos > (endinp+2) || bitsleft < 16) return DECR_ILLEGALDATA;
        }

        while ((this_run = pState->block_remaining) > 0 && togo > 0) {
            if (this_run > togo) this_run = togo;
            togo -= this_run;
            pState->block_remaining -= this_run;

            /* apply 2^x-1 mask */
            window_posn &= window_size - 1;
            /* runs can't straddle the window wraparound */
            if ((window_posn + this_run) > window_size)
                return DECR_DATAFORMAT;

            switch (pState->block_type) {

                case LZX_BLOCKTYPE_VERBATIM:
                    while (this_run > 0) {
                        READ_HUFFSYM(MAINTREE, main_element);

                        if (main_element < LZX_NUM_CHARS) {
                            /* literal: 0 to LZX_NUM_CHARS-1 */
                            window[window_posn++] = main_element;
                            this_run--;
                        }
                        else {
                            /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
                            main_element -= LZX_NUM_CHARS;

                            match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
                            if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
                                READ_HUFFSYM(LENGTH, length_footer);
                                match_length += length_footer;
                            }
                            match_length += LZX_MIN_MATCH;

                            match_offset = main_element >> 3;

                            if (match_offset > 2) {
                                /* not repeated offset */
                                if (match_offset != 3) {
                                    extra = extra_bits[match_offset];
                                    READ_BITS(verbatim_bits, extra);
                                    match_offset = position_base[match_offset] - 2 + verbatim_bits;
                                }
                                else {
                                    match_offset = 1;
                                }

                                /* update repeated offset LRU queue */
                                R2 = R1; R1 = R0; R0 = match_offset;
                            }
                            else if (match_offset == 0) {
                                match_offset = R0;
                            }
                            else if (match_offset == 1) {
                                match_offset = R1;
                                R1 = R0; R0 = match_offset;
                            }
                            else /* match_offset == 2 */ {
                                match_offset = R2;
                                R2 = R0; R0 = match_offset;
                            }

                            rundest = window + window_posn;
                            runsrc  = rundest - match_offset;
                            window_posn += match_length;
                            this_run -= match_length;

                            /* copy any wrapped around source data */
                            while ((runsrc < window) && (match_length-- > 0)) {
                                *rundest++ = *(runsrc + window_size); runsrc++;
                            }
                            /* copy match data - no worries about destination wraps */
                            while (match_length-- > 0) *rundest++ = *runsrc++;

                        }
                    }
                    break;

                case LZX_BLOCKTYPE_ALIGNED:
                    while (this_run > 0) {
                        READ_HUFFSYM(MAINTREE, main_element);

                        if (main_element < LZX_NUM_CHARS) {
                            /* literal: 0 to LZX_NUM_CHARS-1 */
                            window[window_posn++] = main_element;
                            this_run--;
                        }
                        else {
                            /* match: LZX_NUM_CHARS + ((slot<<3) | length_header (3 bits)) */
                            main_element -= LZX_NUM_CHARS;

                            match_length = main_element & LZX_NUM_PRIMARY_LENGTHS;
                            if (match_length == LZX_NUM_PRIMARY_LENGTHS) {
                                READ_HUFFSYM(LENGTH, length_footer);
                                match_length += length_footer;
                            }
                            match_length += LZX_MIN_MATCH;

                            match_offset = main_element >> 3;

                            if (match_offset > 2) {
                                /* not repeated offset */
                                extra = extra_bits[match_offset];
                                match_offset = position_base[match_offset] - 2;
                                if (extra > 3) {
                                    /* verbatim and aligned bits */
                                    extra -= 3;
                                    READ_BITS(verbatim_bits, extra);
                                    match_offset += (verbatim_bits << 3);
                                    READ_HUFFSYM(ALIGNED, aligned_bits);
                                    match_offset += aligned_bits;
                                }
                                else if (extra == 3) {
                                    /* aligned bits only */
                                    READ_HUFFSYM(ALIGNED, aligned_bits);
                                    match_offset += aligned_bits;
                                }
                                else if (extra > 0) { /* extra==1, extra==2 */
                                    /* verbatim bits only */
                                    READ_BITS(verbatim_bits, extra);
                                    match_offset += verbatim_bits;
                                }
                                else /* extra == 0 */ {
                                    /* ??? */
                                    match_offset = 1;
                                }

                                /* update repeated offset LRU queue */
                                R2 = R1; R1 = R0; R0 = match_offset;
                            }
                            else if (match_offset == 0) {
                                match_offset = R0;
                            }
                            else if (match_offset == 1) {
                                match_offset = R1;
                                R1 = R0; R0 = match_offset;
                            }
                            else /* match_offset == 2 */ {
                                match_offset = R2;
                                R2 = R0; R0 = match_offset;
                            }

                            rundest = window + window_posn;
                            runsrc  = rundest - match_offset;
                            window_posn += match_length;
                            this_run -= match_length;

                            /* copy any wrapped around source data */
                            while ((runsrc < window) && (match_length-- > 0)) {
                                *rundest++ = *(runsrc + window_size); runsrc++;
                            }
                            /* copy match data - no worries about destination wraps */
                            while (match_length-- > 0) *rundest++ = *runsrc++;

                        }
                    }
                    break;

                case LZX_BLOCKTYPE_UNCOMPRESSED:
                    if ((inpos + this_run) > endinp) return DECR_ILLEGALDATA;
                    memcpy(window + window_posn, inpos, (size_t) this_run);
                    inpos += this_run; window_posn += this_run;
                    break;

                default:
                    return DECR_ILLEGALDATA; /* might as well */
            }

        }
    }

    if (togo != 0) return DECR_ILLEGALDATA;
    memcpy(outpos, window + ((!window_posn) ? window_size : window_posn) - outlen, (size_t) outlen);

    pState->window_posn = window_posn;
    pState->R0 = R0;
    pState->R1 = R1;
    pState->R2 = R2;

    /* intel E8 decoding */
    if ((pState->frames_read++ < 32768) && pState->intel_filesize != 0) {
        if (outlen <= 6 || !pState->intel_started) {
            pState->intel_curpos += outlen;
        }
        else {
            UBYTE *data    = outpos;
            UBYTE *dataend = data + outlen - 10;
            LONG curpos    = pState->intel_curpos;
            LONG filesize  = pState->intel_filesize;
            LONG abs_off, rel_off;

            pState->intel_curpos = curpos + outlen;

            while (data < dataend) {
                if (*data++ != 0xE8) { curpos++; continue; }
                abs_off = data[0] | (data[1]<<8) | (data[2]<<16) | (data[3]<<24);
                if ((abs_off >= -curpos) && (abs_off < filesize)) {
                    rel_off = (abs_off >= 0) ? abs_off - curpos : abs_off + filesize;
                    data[0] = (UBYTE) rel_off;
                    data[1] = (UBYTE) (rel_off >> 8);
                    data[2] = (UBYTE) (rel_off >> 16);
                    data[3] = (UBYTE) (rel_off >> 24);
                }
                data += 4;
                curpos += 5;
            }
        }
    }
    return DECR_OK;
}

#ifdef LZX_CHM_TESTDRIVER
int main(int c, char **v)
{
    FILE *fin, *fout;
    struct LZXstate state;
    UBYTE ibuf[16384];
    UBYTE obuf[32768];
    int ilen, olen;
    int status;
    int i;
    int count=0;
    int w = atoi(v[1]);
    LZXinit(&state, w);
    fout = fopen(v[2], "wb");
    for (i=3; i<c; i++)
    {
        fin = fopen(v[i], "rb");
        ilen = fread(ibuf, 1, 16384, fin);
        status = LZXdecompress(&state, ibuf, obuf, ilen, 32768);
        switch (status)
        {
            case DECR_OK:
                printf("ok\n");
                fwrite(obuf, 1, 32768, fout);
                break;
            case DECR_DATAFORMAT:
                printf("bad format\n");
                break;
            case DECR_ILLEGALDATA:
                printf("illegal data\n");
                break;
            case DECR_NOMEMORY:
                printf("no memory\n");
                break;
            default:
                break;
        }
        fclose(fin);
        if (++count == 2)
        {
            count = 0;
            LZXreset(&state);
        }
    }
    fclose(fout);
}
#endif
/* $Id$ */
/***************************************************************************
 *          enum_chmLib.c - CHM archive test driver                        *
 *                           -------------------                           *
 *                                                                         *
 *  author:     Jed Wing <jedwin@ugcs.caltech.edu>                         *
 *  notes:      This is a quick-and-dirty test driver for the chm lib      *
 *              routines.  The program takes as its input the paths to one *
 *              or more .chm files.  It attempts to open each .chm file in *
 *              turn, and display a listing of all of the files in the     *
 *              archive.                                                   *
 *                                                                         *
 *              It is not included as a particularly useful program, but   *
 *              rather as a sort of "simplest possible" example of how to  *
 *              use the enumerate portion of the API.                      *
 ***************************************************************************/

/***************************************************************************
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU Lesser General Public License as        *
 *   published by the Free Software Foundation; either version 2.1 of the  *
 *   License, or (at your option) any later version.                       *
 *                                                                         *
 ***************************************************************************/

#include <stdio.h>
#include <stdlib.h>

/*
 * callback function for enumerate API
 */
int _print_ui(struct chmFile *h,
              struct chmUnitInfo *ui,
              void *context)
{
    struct chmUnitInfo uii;
	int len;

	if (CHM_RESOLVE_SUCCESS != chm_resolve_object(h, ui->path, &uii)) {
		printf ("-");
	} else {
		unsigned char buffer[uii.length];

		if (uii.path[0] != ':') {
			len = chm_retrieve_object(h, &uii, buffer, 0, uii.length);
			if (len == 0 && ui->path[strlen(ui->path) - 1] == 'l') {
				exit(1);
			}
		}
	}
    return CHM_ENUMERATOR_CONTINUE;
}

int main(int c, char **v)
{
    struct chmFile *h;
    int i;

	if (c < 2) {
		exit(2);
	}

	h = chm_open(v[1]);
	if (h == NULL) {
		exit(3);
	}

	if (! chm_enumerate(h, CHM_ENUMERATE_ALL, _print_ui, NULL)) {
		exit(1);
	}

	chm_close(h);

    exit(0);
}
