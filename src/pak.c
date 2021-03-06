﻿/***********************************************************************************
 * Copyright (c) 2013, baickl(baickl@gmail.com)
 * All rights reserved.
 * 
 * Redistribution and use in source and binary forms, with or without modification,
 * are permitted provided that the following conditions are met:
 * 
 * * Redistributions of source code must retain the above copyright notice, this
 * list of conditions and the following disclaimer.
 * 
 * * Redistributions in binary form must reproduce the above copyright notice, this
 * list of conditions and the following disclaimer in the documentation and/or
 * other materials provided with the distribution.
 * 
 * * Neither the name of the {organization} nor the names of its
 * contributors may be used to endorse or promote products derived from
 * this software without specific prior written permission.
 * 
 * THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND
 * ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED
 * WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE
 * DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR
 * ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES
 * (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES;
 * LOSS OF USE, DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON
 * ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 * (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS
 * SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
***********************************************************************************/
#include <vfs/util.h>
#include <vfs/base.h>
#include "pak.h"
#include "vfs_private.h"
#include "pool.h"
#include <stdio.h>
#include <memory.h>

static FILE* sfopen(const VFS_CHAR* filename,const VFS_CHAR* mode)
{
#ifdef __linux__
	return fopen(filename,mode);
#else
	FILE* fp = NULL;
	VFS_INT32 err;

	err = fopen_s(&fp,filename,mode);
	if( err == 0 )
	{
		return fp;
	}
	else
	{
		return NULL;
	}
#endif
}


static vfs_plugin* vfs_plugin_get_compress_type(const VFS_CHAR* compress_type)
{
    int i;
    vfs_plugin* plugin;

    if( !g_vfs || !compress_type )
        return NULL;

    for( i = 0; i<g_vfs->_M_plugins_count; ++i )
    {
        plugin = g_vfs->_M_plugins[i];
        if( plugin->type != PLUGIN_COMPRESS)
            continue;

        if( stricmp(compress_type,plugin->info.get_plugin_name() ) == 0 )
            return plugin;
    }

    return NULL;
}


pak* pak_open(const VFS_CHAR* _pakfile,const VFS_CHAR* _prefix)
{
	VFS_INT32 i;
	pak *_pak = NULL;
	FILE *fp = NULL;
	pak_header header;
	pak_iteminfo*iteminfos = NULL;
	VFS_UINT16 filenamelen;
    VFS_CHAR filename[VFS_MAX_FILENAME+1];

    VFS_INT32 prefixlen  = 0;
    VFS_INT32 keylen = 0;
    VFS_CHAR* key = NULL;

    VFS_UINT16 compress_plugin_namelen;
    VFS_CHAR compress_plugin_name[PAK_MAX_PLUGIN_LEN+1];

    struct hashtable *ht_iteminfos = NULL;

    if( !g_vfs )
        return NULL;

    if( _prefix )
        prefixlen = strlen(_prefix);

	/*
	 * 打开文件
	 * */
	fp = sfopen(_pakfile,"rb");
	if( !fp )
		return NULL;

	/* 
	 * 校验HEADER头
	 * */
	if( fread(&header._M_flag,1,sizeof(header._M_flag),fp) != sizeof(header._M_flag))
		goto ERROR;

	if( header._M_flag != MAKE_CC_ID('p','a','k','x'))
		goto ERROR;

	if( fread(&header._M_version,1,sizeof(header._M_version),fp) != sizeof(header._M_version))
		goto ERROR;

	if( header._M_version < PAK_VERSION )
		goto ERROR;

	if( fread(&header._M_count,1,sizeof(header._M_count),fp) != sizeof(header._M_count))
		goto ERROR;

	if( header._M_count <= 0 )
		goto ERROR;	

	if( fread(&header._M_offset,1,sizeof(header._M_offset),fp) != sizeof(header._M_offset))
		goto ERROR;

	if( header._M_offset <= 0 )
		goto ERROR;	

	/* 
	 * 提取文件信息
	 * */
    ht_iteminfos = create_hashtable(header._M_count,pak_item_hashcode,pak_item_equalkeys,pak_item_key_free);
    if( !ht_iteminfos )
        goto ERROR;    


	for( i = 0; i<header._M_count; ++i )
	{

        iteminfos = (pak_iteminfo*)vfs_pool_malloc(sizeof(pak_iteminfo));
        if( !iteminfos )
            goto ERROR;

		if( fread(&iteminfos->_M_offset,1,sizeof(iteminfos->_M_offset),fp) != sizeof(iteminfos->_M_offset))
			goto ERROR;

		iteminfos->_M_offset += header._M_offset; 
		
		if( fread(&iteminfos->_M_size,1,sizeof(iteminfos->_M_size),fp) != sizeof(iteminfos->_M_size))
			goto ERROR;

		if( fread(&iteminfos->_M_crc32,1,sizeof(iteminfos->_M_crc32),fp) != sizeof(iteminfos->_M_crc32))
			goto ERROR;	

        compress_plugin_namelen = 0;
        if( fread(&compress_plugin_namelen,1,sizeof(compress_plugin_namelen),fp) != sizeof(compress_plugin_namelen))
            goto ERROR;

        if( compress_plugin_namelen < 0 || compress_plugin_namelen > PAK_MAX_PLUGIN_LEN )
            goto ERROR;

        memset(compress_plugin_name,0,sizeof(compress_plugin_name));
        if( compress_plugin_namelen > 0 )
        {
            if( fread(compress_plugin_name,1,compress_plugin_namelen,fp) != compress_plugin_namelen )
                goto ERROR;
        }

        strcpy(iteminfos->_M_compress_plugin,compress_plugin_name);

		if( fread(&iteminfos->_M_compress_size,1,sizeof(iteminfos->_M_compress_size),fp) != sizeof(iteminfos->_M_compress_size))
			goto ERROR;	

		filenamelen = 0;
		if( fread(&filenamelen,1,sizeof(filenamelen),fp) != sizeof(filenamelen))
			goto ERROR;

		if( filenamelen <= 0 || filenamelen >= VFS_MAX_FILENAME )
			goto ERROR;

        memset(filename,0,sizeof(filename));
        if( fread(filename,1,filenamelen,fp) != filenamelen )
            goto ERROR;

        keylen = filenamelen + prefixlen;
        key = (VFS_CHAR*)vfs_pool_malloc(keylen+1);
        if( !key )
            goto ERROR;
		
        memset(key,0,keylen);
        if( prefixlen > 0  )
        {            
            if( !vfs_util_path_combine(key,_prefix,filename))
                goto ERROR;
        }
        else
        {
            if( !vfs_util_path_clone(key,filename) )
                goto ERROR;
        }

        vfs_util_str_tolower(key);
        if( !pak_item_insert(ht_iteminfos,key,iteminfos))
        {
            if( key )
            {
                vfs_pool_free(key);
                key = NULL;
            }
            goto ERROR;
        }
        
        /* 清数据 */
        key = NULL;
        keylen = 0;
	}

	VFS_SAFE_FCLOSE(fp);


	/* 
	 * 读取成功，组织文件包
	 * */
	_pak = (pak*)vfs_pool_malloc(sizeof(pak));
	if( !_pak )
		goto ERROR;
	
	memset(_pak->_M_filename,0,sizeof(_pak->_M_filename));
	memset(&_pak->_M_header,0,sizeof(_pak->_M_header));
	strcpy(_pak->_M_filename,_pakfile);
	memcpy(&_pak->_M_header,&header,sizeof(header));
    _pak->_M_ht_iteminfos = ht_iteminfos;

	return _pak;

ERROR:
	VFS_SAFE_FCLOSE(fp);
	if(iteminfos)
    {
        vfs_pool_free(iteminfos);
        iteminfos = NULL;
    }

    if( ht_iteminfos )
    {
        hashtable_destroy(ht_iteminfos);
        ht_iteminfos = NULL;
    }

	return NULL;
}

VFS_VOID pak_close(pak* _pak)
{
    struct hashtable_itr *itr;
    pak_iteminfo * iteminfo;
	
    if( !_pak )
		return;

    if( _pak->_M_ht_iteminfos ){
        
        itr = hashtable_iterator_create(_pak->_M_ht_iteminfos);
        if (itr && hashtable_count(_pak->_M_ht_iteminfos) > 0) 
        {
            do 
            {
                iteminfo =(pak_iteminfo*) hashtable_iterator_value(itr);
                if(iteminfo)
                {
                    vfs_pool_free(iteminfo);
                    iteminfo = NULL;
                }

            } while (hashtable_iterator_remove(itr));
        }

        hashtable_iterator_destroy(itr);
        hashtable_destroy(_pak->_M_ht_iteminfos);
        _pak->_M_ht_iteminfos = NULL;
    }

	if(_pak)
    {
        vfs_pool_free(_pak);
        _pak = NULL;
    }
}


VFS_INT32 pak_item_get_count(pak* _pak)
{
	if( !_pak)
		return 0;

	return _pak->_M_header._M_count;
}


VFS_BOOL pak_item_foreach( pak* _pak,pak_item_foreach_proc proc,VFS_VOID*p )
{
    VFS_INT32 ret;
    int index;
    struct hashtable_itr *itr;
    VFS_CHAR *filename;
    pak_iteminfo * iteminfo;
    if( !_pak || !_pak->_M_ht_iteminfos )
        return VFS_FALSE;

    if( !proc )
        return VFS_FALSE;

    index = 0;
    itr = hashtable_iterator_create(_pak->_M_ht_iteminfos);
    if( !itr )
        return VFS_FALSE;

    if (hashtable_count(_pak->_M_ht_iteminfos) > 0)
    {
        do 
        {
            filename = (VFS_CHAR*)hashtable_iterator_key(itr);
            iteminfo =(pak_iteminfo*) hashtable_iterator_value(itr);
            ret = proc(_pak,filename,iteminfo,index++,p);
            switch(ret)
            {
            case VFS_FOREACH_BREAK:
                hashtable_iterator_destroy(itr);
                return VFS_TRUE;
            case VFS_FOREACH_CONTINUE:
            case VFS_FOREACH_IGNORE:
                break;
            default:
                break; 
            }

        } while (hashtable_iterator_advance(itr));
    }

    hashtable_iterator_destroy(itr);
    return VFS_TRUE;

}

pak_iteminfo *pak_item_locate(pak*_pak,const VFS_CHAR* _file)
{
	pak_iteminfo* iteminfo=NULL;
    VFS_CHAR file[VFS_MAX_FILENAME+1];

	if( !_pak || !_file)
		return NULL;

    vfs_util_path_clone(file,_file);
	vfs_util_path_checkfix(file);
    vfs_util_str_tolower(file);
    
    iteminfo = pak_item_search( _pak->_M_ht_iteminfos,file);
    return iteminfo;

}

VFS_BOOL pak_item_unpack_filename(pak*_pak,const VFS_CHAR*_file,VFS_VOID*_buf,VFS_UINT64 _bufsize)
{
	pak_iteminfo* iteminfo = NULL;
	FILE* fp = NULL;
	VFS_VOID* compress_buf = NULL;

    vfs_plugin* plugin;

	VFS_UINT32 crc32;

	if( !_pak || !_buf || !_file)
		return VFS_FALSE;

	iteminfo = pak_item_locate(_pak,_file);
	if( !iteminfo )
		return VFS_FALSE;

	if( iteminfo->_M_size > _bufsize )
		return VFS_FALSE;

	/* 
	 * 打开文件尝试读取数据
	 * */

	fp = sfopen(_pak->_M_filename,"rb");
	if( !fp )
		return VFS_FALSE;

	if( VFS_FSEEK(fp,iteminfo->_M_offset,SEEK_SET) != 0)
		goto ERROR;

	if( iteminfo->_M_compress_size == 0 )
    {
		if( fread(_buf,1,(VFS_SIZE)iteminfo->_M_size,fp) != iteminfo->_M_size)
			goto ERROR;

		VFS_SAFE_FCLOSE(fp);
		
		/*
		 * 校验数据是否正确
		 * */
		crc32 = vfs_util_calc_crc32(_buf,(VFS_INT32)iteminfo->_M_size);
		if( crc32 != iteminfo->_M_crc32 )
			goto ERROR;

		return VFS_TRUE;
	} 
    else
    {
        plugin = vfs_plugin_get_compress_type(iteminfo->_M_compress_plugin);
        if( !plugin )
            return VFS_FALSE;

		compress_buf = (VFS_VOID*)vfs_pool_malloc((VFS_SIZE)iteminfo->_M_compress_size);
		if( !compress_buf )
			goto ERROR;

		if( fread(compress_buf,1,(VFS_SIZE)iteminfo->_M_compress_size,fp) != iteminfo->_M_compress_size)
			goto ERROR;

		if( plugin->plugin.compress.decompress( compress_buf , (VFS_SIZE)iteminfo->_M_compress_size , _buf,(VFS_SIZE)_bufsize) != iteminfo->_M_size  )
			goto ERROR;

		if(compress_buf){
            vfs_pool_free(compress_buf);
            compress_buf = NULL;
        }

		VFS_SAFE_FCLOSE(fp);

		if( vfs_util_calc_crc32(_buf,(VFS_INT32)iteminfo->_M_size) != iteminfo->_M_crc32) 
			goto ERROR;

		return VFS_TRUE;
	}
	
ERROR:
	if(compress_buf){
        vfs_pool_free(compress_buf);
        compress_buf = NULL;
    }
	VFS_SAFE_FCLOSE(fp);

	return VFS_FALSE;
}
