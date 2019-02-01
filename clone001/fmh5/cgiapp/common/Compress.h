/*
 * Compress.h
 *
 *  Created on: 2011-7-27
 *      Author: dada
 */

#ifndef COMPRESS_H_
#define COMPRESS_H_

#include "Common.h"

namespace Compress
{
	bool GzipCompress(string &result, const string &text);
	bool GzipUncompress(string &result, const string &text);
};

#endif /* COMPRESS_H_ */
