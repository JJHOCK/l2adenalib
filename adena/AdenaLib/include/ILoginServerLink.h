/*
 * ILoginServerLink.h - Gameserver -> Loginserver communication.
 * Created January 8, 2007, by Michael 'Bigcheese' Spencer.
 *
 * Copyright (C) 2007 Michael Spencer
 *
 * This library is free software; you can redistribute it and/or
 * modify it under the terms of the GNU Lesser General Public
 * License as published by the Free Software Foundation; either
 * version 2.1 of the License, or (at your option) any later version.
 *
 * This library is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU
 * Lesser General Public License for more details.
 * 
 * You should have received a copy of the GNU Lesser General Public
 * License along with this library; if not, write to the Free Software
 * Foundation, Inc., 51 Franklin Street, Fifth Floor, Boston, MA  02110-1301  USA
 *
 * Michael Spencer - bigcheesegs@gmail.com
 */

#ifndef _ADENA_I_LOGIN_SERVER_LINK_H_
#define _ADENA_I_LOGIN_SERVER_LINK_H_

#include <AdenaConfig.h>

namespace adena
{
namespace game_server
{

	/*
	 * This class allows us to hide the interface used between the login server and game server.
	 * All calls are async, so when you call registerWithLoginServer, the LoginServerLink will call
	 * registerWithLoginServerResult(bool) on the game server.
	 */

	enum E_LoginLinkEventType
	{
		ELLET_REGISTER_RESULT = 0
	};

	enum E_LoginLinkResult
	{
		ELLR_OK = 0,
		ELLR_LOGIN_SERVER_DOWN,
		ELLR_AUTH_FAILED
	};

	struct SLoginLinkEvent
	{
		E_LoginLinkEventType EventType;
		E_LoginLinkResult Result;
		void* Data;
	};

	class ILoginServerLink
	{
	public:

		virtual ~ILoginServerLink() {}

		virtual void registerWithLoginServer(irr::c8 ip[4], irr::u16 port) = 0;

	private:

	};

}
}

#endif