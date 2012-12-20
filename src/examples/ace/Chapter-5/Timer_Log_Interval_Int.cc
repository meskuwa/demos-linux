/*
	This file is part of the linuxapi project.
	Copyright (C) 2011, 2012 Mark Veltzer <mark.veltzer@gmail.com>

	The linuxapi package is free software; you can redistribute it and/or
	modify it under the terms of the GNU Lesser General Public
	License as published by the Free Software Foundation; either
	version 2.1 of the License, or (at your option) any later version.

	The linuxapi package is distributed in the hope that it will be useful,
	but WITHOUT ANY WARRANTY; without even the implied warranty of
	MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE. See the GNU
	Lesser General Public License for more details.

	You should have received a copy of the GNU Lesser General Public
	License along with the GNU C Library; if not, write to the Free
	Software Foundation, Inc., 59 Temple Place, Suite 330, Boston, MA
	02111-1307 USA.
*/

#include <firstinclude.h>
#include <ace/OS_NS_time.h>
#include <ace/Log_Msg.h>
#include <ace/Reactor.h>
#include <ace/Event_Handler.h>
#include <ace/Signal.h>
#include <stdlib.h> // for EXIT_SUCCESS

/*
* Reactor examples
* Timers & state data
*
* EXTRA_CMDS=pkg-config --cflags --libs ACE
*/

class TimerHandler:public ACE_Event_Handler {
	public:
		int handle_timeout(const ACE_Time_Value& current_time, const void* arg) {
			ACE_DEBUG((LM_INFO, ACE_TEXT("handle_timeout\n")));
			return(0);
		}
};

class LoopStopper:public ACE_Event_Handler {
	private:
		int timerId_;
	public:
		LoopStopper(int signum,int timerId):timerId_(timerId) {
			ACE_Reactor::instance()->register_handler(signum,this);
		}
		int handle_signal(int, siginfo_t *, ucontext_t *) {
			//ACE_Reactor::instance()->end_reactor_event_loop ();
			ACE_Reactor::instance()->cancel_timer(this->timerId_);
			return 0;
		}
};

class LogSwitcher:public ACE_Event_Handler {
	private:
		int on_sig_; // Signal to turn logging on
		int off_sig_; // Signal to turn logging off
		int on_off_; // 1==turn on, 0==turn off
	public:
		LogSwitcher(int on_sig, int off_sig):on_sig_(on_sig),off_sig_(off_sig) {
			ACE_Sig_Set sigs;
			sigs.sig_add (on_sig);
			sigs.sig_add (off_sig);
			ACE_Reactor::instance()->register_handler(sigs,this);
		}
		int handle_signal(int signum, siginfo_t *, ucontext_t *) {
			if (signum==this->on_sig_ || signum==this->off_sig_) {
				this->on_off_=signum==this->on_sig_;
				ACE_Reactor::instance ()->notify (this);
			}
			return 0;
		}
		int handle_exception(ACE_HANDLE) {
			ACE_DEBUG((LM_INFO, ACE_TEXT("log reaction\n")));
			if (this->on_off_)
				ACE_LOG_MSG->clr_flags(ACE_Log_Msg::SILENT);
			else
				ACE_LOG_MSG->set_flags(ACE_Log_Msg::SILENT);
			return 0;
		}
};

int ACE_TMAIN(int argc,ACE_TCHAR** argv,ACE_TCHAR** envp) {
	ACE_DEBUG((LM_INFO, ACE_TEXT("contact me at process %P\n")));
	TimerHandler* timerHandler=new TimerHandler();
	int timerId=ACE_Reactor::instance()->schedule_timer(
		timerHandler,
		NULL,
		ACE_Time_Value(0),
		ACE_Time_Value(5)
	);
	new LogSwitcher(SIGUSR1,SIGUSR2);
	new LoopStopper(SIGINT,timerId);
	ACE_Reactor::instance()->run_reactor_event_loop();
	return EXIT_SUCCESS;
}