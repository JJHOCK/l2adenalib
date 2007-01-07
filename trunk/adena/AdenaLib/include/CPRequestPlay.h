/*
 * CPRequestPlay.h - After select server.
 * Created January 6, 2007, by Michael 'Bigcheese' Spencer.
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

#ifndef _ADENA_C_P_REQUEST_PLAY_H_
#define _ADENA_C_P_REQUEST_PLAY_H_

#include <CPRequestLogin.h>

namespace adena
{

	class CPRequestPlay : public CLoginServerPacket
	{
	public:

		CPRequestPlay(irr::c8* in_data)
		: CLoginServerPacket()
		{
			Data = in_data;
			Data++;
			r32();
			r32();
			ServerIndex = r8();
		};

		virtual ~CPRequestLogin()
		{
			
		};

		virtual irr::c8* getData()
		{
			return NULL;
		};

		virtual irr::u32 getLen()
		{
			return 0;
		};

		irr::u8 ServerIndex;

	private:



	};

}

#endif
