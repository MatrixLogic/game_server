/*
 * DataNoise.h
 *
 *  Created on: 2013-11-07
 *      Author: Ralf
 */

#ifndef DATANOISE_H_
#define DATANOISE_H_

#include "Kernel.h"
#include "DataInc.h"

#define NOISE_NUM 1000
#define NOISE_COUNT 5
#define NOISE_TIME 600
#define NOISE_SHUT 86400

struct Noise
{
	unsigned uid;
	unsigned ts;
	unsigned count[10];
};

struct DataNoise {
	Noise noise[NOISE_NUM];
};

class CDataNoise {
public:
	CDataNoise();
	int Init(const string &path, semdat sem=sem_noise);
	int Add(unsigned uid,unsigned uidby);

protected:
	CShareMemory m_sh;
	bool m_init;
};

#endif /* DATANOISE_H_ */
