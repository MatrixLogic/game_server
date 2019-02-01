#ifndef _SERVER_INC_H_
#define _SERVER_INC_H_

#define RMI ResourceManager::Instance()
#define SEQ_GET_RMI(seq) RMI->m_data->item[seq]
#define GET_RMI(uid) SEQ_GET_RMI(RMI->GetIndex(uid))
#define UMI UserManager::Instance()
#define LMI LogicManager::Instance()
#define CMI ConfigManager::Instance()

#include "Kernel.h"
#include "DataInc.h"
#include "LogicManager.h"
#include "UserManager.h"
#include "LogicResourceManager.h"
#include "LogicUserManager.h"
#include "LogicNotifyManager.h"
#include "LogicGameActivityManager.h"
#include "LogicRoutineManager.h"
#include "LogicBuildingManager.h"
#include "ParseManager.h"
#include "LogicPropsManager.h"
#include "LogicProductLineManager.h"
#include "LogicQueueManager.h"
#include "LogicGM.h"

#endif
