/*
 * LogicGM.cpp
 *
 *  Created on: 2017-7-21
 *      Author: dawx62fac
 */

#include "LogicGM.h"


void GMCmd::split(std::string text, const char key)
{
	v_args_.clear();
	do
	{
		text.erase(0, text.find_first_not_of(key));
		if (text.empty())
		{
			break;
		}

		std::string::size_type p = text.find_first_of(key);
		v_args_.push_back(text.substr(0, p));
		text.erase(0, p);

	} while (1);

}


template<>
int GMCmd::get_arg<int>(int index) const
{
	check_args_index(index);

	int v = 0;
	if (Convert::StringToInt(v, v_args_[index + 1]))
	{
		return v;
	}
	else
	{
		throw std::runtime_error("the_args_not_int_type");
	}
}


template<>
std::string GMCmd::get_arg<std::string>(int index) const
{
	check_args_index(index);

	return v_args_[index + 1];
}

////////////////////////////////////////////////////////////////////////////////////
LogicGM::LogicGM()
{
}

bool LogicGM::HandleUserBase(const GMCmd& gm, DBCUserBaseWrap& user)
{
	CommonGiftConfig::BaseItem  baseitem;

	if (gm.cmd() == string("pass"))
	{
		//user.Obj().npc_pass = gm.get_arg<int>(0);
	}
	else if (gm.cmd() == string("cash"))
	{
		baseitem.set_cash(gm.get_arg<int>(0));
	}
	else if (gm.cmd() == string("coin"))
	{
		baseitem.set_coin(gm.get_arg<int>(0));
	}
	else if (gm.cmd() == string("level"))
	{
		int new_level = gm.get_arg<int>(0);
		const UserCfg::User& userCfg = UserCfgWrap().User();
		//更新用户level数据
		int levelSize = userCfg.user_exp_size();
		if (new_level > 0 && new_level < levelSize )
		{
			unsigned old_level = user.Obj().level;
			user.Obj().exp =  userCfg.user_exp(new_level - 1);
			user.Obj().level = new_level;

			//user.OnUserUpgradeReward(old_level);
		}
		else
		{
			throw std::runtime_error("param error");
		}

		return true;
	}
	else if (gm.cmd() == string("exp"))
	{
		baseitem.set_exp(gm.get_arg<int>(0));
	}
	else
	{
		return false;
	}

	user.BaseProcess(baseitem, msg_.mutable_common()->mutable_userbase(), "GM");

	return true;
}

bool LogicGM::HandleProps(const GMCmd& gm, unsigned uid)
{
	if (gm.cmd() == string("equip"))
	{
		unsigned eq_id = gm.get_arg<int>(0);
		unsigned count = gm.get_arg<int>(1);
		if (count >= 100000) count = 100000;
		LogicPropsManager::Instance()->AddProps(uid, eq_id, count, "gm_op", msg_.mutable_common()->mutable_props());
	}
	else
	{
		return false;
	}

	return true;
}

void LogicGM::SyncInfo(unsigned uid)
{
	try
	{
		LogicManager::Instance()->sendMsg(uid, &msg_, false);
	}
	catch(const std::exception& e)
	{
		error_log("uid:%u, %s", uid, e.what());
	}
}

int LogicGM::Process(unsigned uid, ProtoGM::GMCmdReq* req)
{
	msg_.Clear();

	DBCUserBaseWrap user(uid);
	if (user.Obj().register_platform != 0)
	{
		throw std::runtime_error("gm_only_support_test_platform");
	}

	GMCmd gm(req->cmd());

	do
	{
		if (HandleUserBase(gm, user)) break;
		if (HandleProps(gm, uid)) break;

		throw std::runtime_error("unknown gm_cmd");

	} while(0);

	if (msg_.ByteSize() > 0)
	{
		SyncInfo(uid);
	}

	return 0;
}
