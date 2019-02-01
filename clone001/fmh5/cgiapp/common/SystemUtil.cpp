/*
 * SystemUtil.cpp
 *
 *  Created on: 2012-1-31
 *      Author: dada
 */

#include "SystemUtil.h"
#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <signal.h>
#include <sys/stat.h>

namespace System
{
	static SignalCallback s_sigusr1 = NULL;
	static SignalCallback s_sigusr2 = NULL;

	static void OnSigusr1(int n)
	{
		if(System::s_sigusr1 != NULL)
		{
			System::s_sigusr1();
			signal(SIGUSR1, System::OnSigusr1);
		}
	}

	static void OnSigusr2(int n)
	{
		if(System::s_sigusr2 != NULL)
		{
			System::s_sigusr2();
			signal(SIGUSR1, System::OnSigusr2);
		}
	}
}

bool System::InitDaemon(SignalCallback sigusr1, SignalCallback sigusr2)
{
	pid_t pid;

	pid = fork();
	if(pid != 0)
	{
		if(pid < 0)
		{
			return false;
		}
		else
		{
			exit(0);
		}
	}

	setsid();

	signal(SIGINT,  SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	signal(SIGQUIT, SIG_IGN);
	signal(SIGPIPE, SIG_IGN);
	signal(SIGTTOU, SIG_IGN);
	signal(SIGTTIN, SIG_IGN);
	signal(SIGCHLD, SIG_IGN);
	signal(SIGTERM, SIG_IGN);
	signal(SIGHUP,  SIG_IGN);
	IgnoreSignal(SIGPIPE);

	pid = fork();
	if(pid != 0)
	{
		if(pid < 0)
		{
			return false;
		}
		else
		{
			exit(0);
		}
	}

	chdir("/");
	umask(0);

	int f = open("/dev/null", O_RDWR);
	if(f != -1)
	{
		dup2(f, STDIN_FILENO);
		dup2(f, STDOUT_FILENO);
		dup2(f, STDERR_FILENO);
	}

	if(sigusr1 != NULL)
	{
		System::s_sigusr1 = sigusr1;
		signal(SIGUSR1, System::OnSigusr1);
	}
	else
	{
		signal(SIGUSR1,  SIG_IGN);
	}
	if(sigusr2 != NULL)
	{
		System::s_sigusr2 = sigusr2;
		signal(SIGUSR2, System::OnSigusr2);
	}
	else
	{
		signal(SIGUSR2,  SIG_IGN);
	}

	return true;
}

bool System::IgnoreSignal(int signum)
{
	struct sigaction sig;
	sig.sa_handler = SIG_IGN;
	sig.sa_flags = 0;
	sigemptyset(&sig.sa_mask);
	return sigaction(signum, &sig, NULL) == 0;
}

volatile bool SysSig::sys_exit_sig = false;
volatile bool SysSig::sys_working_sig = false;

void SysSig::SysOnExitSig(int n)
{
	if(!sys_working_sig)
	{
		sys_working_sig = true;
		exit(0);
	}
	else
		sys_exit_sig = true;
}
void SysSig::SysRegExitSig()
{
	signal(SIGUSR2, SysSig::SysOnExitSig);
}
void SysSig::SysStartWorking()
{
	sys_working_sig = true;
}
void SysSig::SysCheckExit()
{
	if(sys_exit_sig)
		exit(0);
	sys_working_sig = false;
}
