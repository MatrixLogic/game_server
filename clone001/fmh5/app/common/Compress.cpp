/*
 * Compress.cpp
 *
 *  Created on: 2011-7-27
 *      Author: dada
 */

#include "Compress.h"
#include "zlib.h"

bool Compress::GzipCompress(string &result, const string &text)
{
	unsigned buflen = text.size()+1024;
	char *buffer = new char[buflen];
    z_stream strm;
    strm.zalloc = (alloc_func) Z_NULL;
    strm.zfree = (free_func) Z_NULL;
    strm.opaque = Z_NULL;
    if( deflateInit2(&strm, Z_DEFAULT_COMPRESSION, Z_DEFLATED, 16+15, 8, Z_DEFAULT_STRATEGY) != Z_OK )
    {
        return false;
    }
    strm.next_in = const_cast<unsigned char*>((const unsigned char*)text.c_str());
    strm.avail_in = text.size();
    strm.next_out = (unsigned char*)buffer;
    strm.avail_out = buflen;
    deflate(&strm, Z_FINISH);
    deflateEnd(&strm);
    size_t size = buflen - strm.avail_out;
    result.clear();
    result.append(buffer, size);
	delete buffer;
	return true;
}

bool Compress::GzipUncompress(string &result, const string &text)
{
	unsigned bsize = text.size() * 200;
	char *buffer = new char[bsize];
    z_stream strm;
    strm.zalloc = (alloc_func) Z_NULL;
    strm.zfree = (free_func) Z_NULL;
    strm.opaque = Z_NULL;
    if( inflateInit2(&strm, 32+15) != Z_OK )
    {
        return false;
    }
    strm.next_in = const_cast<unsigned char*>((const unsigned char*)text.c_str());
    strm.avail_in = text.size();
    strm.next_out = (unsigned char*)buffer;
    strm.avail_out = bsize;
    inflate(&strm, Z_FINISH);
    inflateEnd(&strm);
    size_t size = bsize - strm.avail_out;
    result.clear();
    result.append(buffer, size);
	delete buffer;
	return true;
}
