/*
 * BufferWriter.h
 *
 *  Created on: 2012-2-8
 *      Author: dada
 */

#ifndef BUFFERWRITER_H_
#define BUFFERWRITER_H_

#include "Common.h"

class CBufferWriter
{
public:
	CBufferWriter();
	CBufferWriter(IBuffer *pBuffer);
	bool Attach(IBuffer *pBuffer);
	bool Detach();
	bool SetSwapByte(bool bSwapByte);	//设置是否自动转换字节序

	bool AppendByte(byte value);
	bool AppendInt16(int16_t value);
	bool AppendUInt16(uint16_t value);
	bool AppendInt32(int32_t value);
	bool AppendUInt32(uint32_t value);
	bool AppendInt64(int64_t value);
	bool AppendUInt64(uint64_t value);
	bool AppendShortString(const string &value);
	bool AppendString(const string &value);
	bool AppendPBMsg(const Message* msg);
	bool AppendBuffer(const IBuffer *value);
	bool AppendBytes(const byte *value, uint32_t size);

private:
	IBuffer *m_pBuffer;
	bool m_bSwapByte;
};

#endif /* BUFFERWRITER_H_ */
