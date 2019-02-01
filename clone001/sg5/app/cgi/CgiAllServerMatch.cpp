/*
 * CgiAllServerMatc.cpp
 *
 *  Created on: 2013-5-14
 *      Author: Administrator
 */


#include "LogicInc.h"

class CCgiAllServerMatch : public CCgiBase
{

public:

	CCgiAllServerMatch() : CCgiBase("allservermatch")
	{
		SetFeature(CF_PRINT_JSON);
		SetFeature(CF_GET_REMOTE_IP);
		SetFeature(CF_CHECK_PLATFORM);
	}

	CGI_SIMPLE_ACTION_MAP_BEGIN
	CGI_SET_ACTION_MAP("viewbasematch", ViewBaseMatch)
	CGI_SET_ACTION_MAP("applybasematch", ApplyBaseMatch)
	CGI_SET_ACTION_MAP("reportbasematch", ReportBaseMatch)
	CGI_SET_ACTION_MAP("viewbaseapplyers", ViewBaseApplyers)
	CGI_SET_ACTION_MAP("viewbaseregular", ViewBaseRegular)
	CGI_SET_ACTION_MAP("viewpersonmatch", ViewPersonMatch)
	CGI_SET_ACTION_MAP("applypersonmatch", ApplyPersonMatch)
	CGI_SET_ACTION_MAP("reportpersonmatch", ReportPersonMatch)
	CGI_SET_ACTION_MAP("guessapply", GuessApply)
	CGI_SET_ACTION_MAP("guessview", GuessView)
	CGI_SET_ACTION_MAP("loadallserverbmatch", LoadBaseMatch)
	CGI_SET_ACTION_MAP("loadallserverpmatch", LoadPersonMatch)
	CGI_SET_ACTION_MAP("viewladder", ViewLadder)
	CGI_SET_ACTION_MAP("refreshladder", RefreshLadder)
	CGI_SET_ACTION_MAP("reportladder", ReportLadder)
	CGI_SET_ACTION_MAP("viewtop1", ViewTopOne)
	CGI_ACTION_MAP_END

	int ViewBaseMatch()
	{
		CLogicBaseMatch logicBaseMatch;
		unsigned aid = CCGIIn::GetCGIInt("aid");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (!IsValidUid(uid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int ret = logicBaseMatch.GetBaseMatchInfo(aid, uid, m_jsonResult,true);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbasematch&uid=%u&aid=%u", uid, aid);
		return R_SUCCESS;
	}

	int ApplyBaseMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (!IsValidUid(uid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.Apply(uid,true);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=applybasematch&uid=%u", uid);
		return R_SUCCESS;
	}

	int ReportBaseMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (!IsValidUid(uid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int order = CCGIIn::GetCGIInt("order");
		int result = CCGIIn::GetCGIInt("result");
		int ret = logicBaseMatch.ReportResult(uid, order, result,true);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportbasematch&uid=%u&order=%d&result=%d", uid, order, result);
		return R_SUCCESS;
	}

	int ViewBaseApplyers()
	{
		unsigned aid = CCGIIn::GetCGIInt("aid");
		if (!IsAllianceId(aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.GetApplyPlayers(aid, m_jsonResult, true);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbaseapplyers&aid=%u", aid);
		return R_SUCCESS;
	}

	int ViewBaseRegular()
	{
		unsigned aid = CCGIIn::GetCGIInt("aid");
		if (!IsAllianceId(aid))
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.GetRegularScore(aid, m_jsonResult, true);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewbaseregular&aid=%u", aid);
		return R_SUCCESS;
	}

	int GuessApply()
	{
		unsigned gid = CCGIIn::GetCGIInt("gid");
		unsigned coins = CCGIIn::GetCGIInt("coins");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int type = CCGIIn::GetCGIInt("type");
		int lev = CCGIIn::GetCGIInt("level");
		if ( coins <= 0 || type > 2 || type < 0)
		{
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		CGI_SEND_LOG("action=guessapply&uid=%u&gid=%d&coins=%d", uid, gid, coins);

		int stage = 10;
		if(type == 0)
		{
			CLogicMatch match;
			if(match.GetStage(stage) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}
		else if(type == 1)
		{
			CLogicBaseMatch match;
			if(match.GetStage(stage, true) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}
		else
		{
			CLogicAllServerPersonMatch match;
			if(match.GetStage(stage,lev) != 0){
				error_log("get stage failed");
				return R_ERR_LOGIC;
			}
		}

		if(stage != 10){
			error_log("stage expect 10 but %d",stage);
			return R_ERR_LOGIC;
		}

		CLogicPay logicPay;
		DataPay data_pay;
		if(logicPay.GetPay(uid,data_pay) != 0)
		{
			error_log("get %u's cash failed",m_uid);
			return R_ERR_DB;
		}
		if(data_pay.coins < unsigned(coins * PER_BET_COINS))
		{
			error_log("user %u's coins not enough[%u,%u]",m_uid,data_pay.coins,coins * PER_BET_COINS);
			return R_ERR_LOGIC;
		}

		CLogicAllServerGuess logicAllServerGuess;
		int ret = logicAllServerGuess.ApplyGuess(uid,gid,coins,type,lev);
		if (0 != ret)
		{
			error_log("user %u apply guess failed [%u %u]", m_uid, gid, coins);
			return R_ERR_LOGIC;
		}

		logicPay.ChangePay(uid,0,coins*-1*PER_BET_COINS,"MATCHGUESSAPPLY",1);
		return R_SUCCESS;
	}

	int GuessView()
	{
		int type = CCGIIn::GetCGIInt("type");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if(type > 2 || type < 0){
			PARAM_ERROR_RETURN_MSG("param_error");
		}
		int level = CCGIIn::GetCGIInt("level");

		CLogicAllServerGuess logicAllServerGuess;
		int ret = logicAllServerGuess.ViewGuess(uid,m_jsonResult,type,level);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=GuessViewA&uid=%u,level=%d",uid,level);
		return 0;
	}

	int ViewPersonMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (!IsValidUid(uid))
		{
			return R_ERR_PARAM;
		}
		int level = 0;
		level = CCGIIn::GetCGIInt("level");
		CLogicAllServerPersonMatch logicPersonMatch;
		int ret = logicPersonMatch.GetBaseMatchInfo(uid, m_jsonResult,level);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewpersonmatch&uid=%u,level=%d", uid, level);
		return 0;
	}

	int ApplyPersonMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		if (!IsValidUid(uid))
		{
			error_log("not valid uid,%u", uid);
			return R_ERR_PARAM;
		}
		int level = CCGIIn::GetCGIInt("level");

		CLogicAllServerPersonMatch logicPersonMatch;
		int ret = logicPersonMatch.Apply(uid,level);
		error_log("[Apply ret=%d]",ret);
		if (ret != 0)
		{
			return ret;
		}
		CGI_SEND_LOG("action=applypersonmatch&uid=%u,level=%d", uid, level);

		return R_SUCCESS;
	}

	int ReportPersonMatch()
	{
		CLogicAllServerPersonMatch logicPersonMatch;
		int order = CCGIIn::GetCGIInt("order");
		int result = CCGIIn::GetCGIInt("result");
		unsigned uid = CCGIIn::GetCGIInt("uid");
		int level = CCGIIn::GetCGIInt("level");
		if (!IsValidUid(uid))
		{
			return R_ERR_PARAM;
		}
		int ret = logicPersonMatch.ReportResult(uid, order, result,level);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportallserverpersonmatch&uid=%u&order=%d&result=%d,level=%d",
				uid, order, result,level);
		return R_SUCCESS;
	}

	int LoadBaseMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned userid = CCGIIn::GetCGIInt("userid");
		if (!IsValidAllServerBMatchInstId(uid))
		{
			return R_ERR_PARAM;
		}
		CLogicBaseMatch logicBaseMatch;
		int ret = logicBaseMatch.Load(uid, userid, m_jsonResult,true);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=loadbasemtach&uid=%u&userid=%u", uid, userid);
		return 0;
	}

	int LoadPersonMatch()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned userid = CCGIIn::GetCGIInt("userid");
		if (!IsValidAllServerPersonMatchId(uid))
		{
			return R_ERR_PARAM;
		}
		CLogicAllServerPersonMatch logicPersonMatch;
		int ret = logicPersonMatch.Load(uid,userid,m_jsonResult);
		if (0 != ret)
		{
			return ret;
		}
		CGI_SEND_LOG("action=loadpersonmtach&uid=%u&userid=%u", uid, userid);
		return 0;
	}

	int ViewLadder()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned level = CCGIIn::GetCGIInt("level");
		CLogicAllServerLadder logicLadder;
		int ret = logicLadder.ViewLadder(uid, m_jsonResult,level);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=viewladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int RefreshLadder()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned level = CCGIIn::GetCGIInt("level");
		CLogicAllServerLadder logicLadder;
		int ret = logicLadder.Refresh(uid, m_jsonResult,level);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=refreshladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ReportLadder()
	{
		unsigned uid = CCGIIn::GetCGIInt("uid");
		unsigned level = CCGIIn::GetCGIInt("level");
		string data = CCGIIn::GetCGIStr("data");
		//debug_log("uid=%u,level=%u,data=%s",uid,level,data.c_str());
		Json::Value jsonData;
		Json::Reader reader;
		reader.parse(data,jsonData);
		CLogicAllServerLadder logicLadder;
		int ret = logicLadder.ReportResult(uid, jsonData, level);
		if (ret != 0)
			return ret;
		CGI_SEND_LOG("action=reportladder&uid=%u",m_uid);
		return R_SUCCESS;
	}

	int ViewTopOne()
	{
		CLogicAllServerLadder logicLadder;
		int ret = logicLadder.ViewTopOne(m_jsonResult["top1"]);
		if (0 != ret)
		{
			return ret;
		}
		return 0;
	}
};

CGI_MAIN(CCgiAllServerMatch)

