/*
 * CLoginServer.cpp - Login server.
 * Created December 31, 2006, by Michael 'Bigcheese' Spencer.
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

#include <CLoginServer.h>
#include <CMersenneTwister.h>
#include <IGameServerLink.h>
#include <CLogger.h>

namespace adena
{
namespace login_server
{

CLoginServer::CLoginServer(irr::net::Address &addr)
: Thread(),  Server(0), Logger(0)
{
	EventParser = new NELoginServerNetEvent(this);
	Server = new irr::net::CTCPServer(EventParser, 10);
	Server->bind(addr);
};

CLoginServer::~CLoginServer()
{
	if(Server)
		delete Server;
	if(Logger)
		delete Logger;
	delete EventParser;
	delete RsaCipher;
	delete BlowfishCipher;
};

void CLoginServer::gameLinkEvent(SGameLinkEvent e)
{
	if(e.EventType == EGLET_REGISTER_REQUEST)
	{
		irr::u32 sID = *((irr::u32*)e.Data);
		printf("Reg request id: %d\n", sID);
	}
};

void CLoginServer::run()
{
	if(Logger == 0)
		Logger = new irr::CLogger(NULL);
	ServerListPacket = new CPServerList();
	Rng = new irr::CMersenneTwister();
	Rng->seed();
	Logger->log("Generating RSA key...");
	RsaCipher = new BDRSA(1024, 65537);
	RsaCipher->getMod(ScrambledMod, 128);
	ScrambleRsaPublicMod();
	Logger->log("RSA key generated and scrambled");
	BlowfishCipher = new CBlowfish("_;5.]94-31==-%xT!^[$\000");
	DataBase = new irr::db::CSQLLite();
	irr::db::CSQLLiteConParms qp = irr::db::CSQLLiteConParms();
	qp.FileName = "l2adena.sqlite";
	Logger->log("Attempting to connect to DB...");
	if(!DataBase->connect(&qp))
		Logger->log("FATAL ERROR: Failed to connect to DB (Check connection settings)", irr::ELL_ERROR);
	else
		Logger->log("DB connection sucsessfull");
	Server->start();
	Logger->log("Login Server up and awaiting connections");
	while(Server->Running)
		irr::core::threads::sleep(1000);
};

bool CLoginServer::loginAccount(irr::u32 account_id, irr::net::IServerClient* client)
{
	SAccountLocation al;
	AccountLocationsMutex.getLock();
	if(AccountLocations.Find(account_id, al))
	{
		if(al.Local == true)
		{
			AccountLocationsMutex.releaseLock();
			Server->kickClient(al.Data);
		}else
		{
			AccountLocationsMutex.releaseLock();
			//GameServerLink->requestKick();
		}
		return false;
	}else
	{
		al.Local = true;
		al.Data = client;
		AccountLocations.Insert(account_id, al);
		AccountLocationsMutex.releaseLock();
		return true;
	}
};

void CLoginServer::unlogAccount(irr::u32 account_id)
{
	AccountLocationsMutex.getLock();
	AccountLocations.Remove(account_id);
	AccountLocationsMutex.releaseLock();
};

SLoginServerStatus CLoginServer::getStatus()
{
	ServerStatus.UpTime = (irr::u32)(time(NULL) - StartTime);
	return ServerStatus;
};

void CLoginServer::ScrambleRsaPublicMod()
{
	char* n = ScrambledMod;
	int i;
	for (i = 0; i < 4; i++)
	{
		byte temp = n[0x00 + i];
		n[0x00 + i] = n[0x4d + i];
		n[0x4d + i] = temp;
	}

	// step 2 xor first 0x40 bytes with last 0x40 bytes
	for (i = 0; i < 0x40; i++)
	{
		n[i] = (byte)(n[i] ^ n[0x40 + i]);
	}

	// step 3 xor bytes 0x0d-0x10 with bytes 0x34-0x38
	for (i = 0; i < 4; i++)
	{
		n[0x0d + i] = (byte)(n[0x0d + i] ^ n[0x34 + i]);
	}

	// step 4 xor last 0x40 bytes with first 0x40 bytes
	for (i = 0; i < 0x40; i++)
	{
		n[0x40 + i] = (byte)(n[0x40 + i] ^ n[i]);
	}
};

}
}
