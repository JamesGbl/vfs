#ifndef _VFS_UITL_H_
#define _VFS_UTIL_H_

#include "common.h"

#define DIR_FOREACH_IGNORE		0	/* ����FOREACH�����ν�����������������Ŀ¼���أ����ʾ������Ŀ¼����FOREACH */
#define DIR_FOREACH_CONTINUE	1   /* ����FOREACH����Ҫ���ڴ���Ŀ¼����*/
#define DIR_FOREACH_BREAK		2   /* �ж�FOREACH��һ�����ҵ���Ҫ���ļ��л����ļ��󣬲���Ҫ����FOREACH�� */

typedef var32 (*dir_foreach_item_proc)(const char*fullpath,var32 dir);

VFS_EXTERN uvar32			vfs_util_calc_crc32( void*, var32 );

VFS_EXTERN var32			vfs_util_compress_bound( var8 ,var32 );
VFS_EXTERN uvar64			vfs_util_compress( var32,  const void*, uvar64,void*, uvar64 );
VFS_EXTERN uvar64			vfs_util_decompress( var32, const void*, uvar64,void*, uvar64 );

VFS_EXTERN VFS_BOOL			vfs_util_dir_foreach(const char*,dir_foreach_item_proc);

VFS_EXTERN char*			vfs_util_path_checkfix(char*);
VFS_EXTERN char*			vfs_util_path_clone(char*,const char*);
VFS_EXTERN char*			vfs_util_path_append(char*,char*);
VFS_EXTERN char*			vfs_util_path_join(char*,char*);
VFS_EXTERN char*			vfs_util_path_combine(char*,const char*,const char*);
VFS_EXTERN char*			vfs_util_path_add_backslash(char*);
VFS_EXTERN char*			vfs_util_path_remove_backslash(char*);
VFS_EXTERN char*			vfs_util_path_remove_filename(char*);
VFS_EXTERN char*			vfs_util_path_remove_extension(char*);

#endif/* _VFS_UTIL_H_ */
