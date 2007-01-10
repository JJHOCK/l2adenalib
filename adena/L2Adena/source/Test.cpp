#include <CLoginServer.h>
#include <CGameServer.h>
#include <irrNet.h>
#include <iostream>
#include <CReadFile.h>
#include <CCrypt.h>
#include <CFileSystem.h>
#include <CInProcessLoginServerLink.h>
#include <CInProcessGameServerLink.h>

using namespace std;
using namespace irr;

static const irr::c8 key[8] = {0x94, 0x35, 0x00, 0x00, 0xa1, 0x6c,	0x54, 0x87};

u32 addPack(io::IFileSystem* fs, c8* buff, c8* file_name)
{
	io::IReadFile* rf = fs->createAndOpenFile(file_name);
	u32 size = rf->getSize();
	return rf->read(buff, size);
}

void parseBuff(c8* buff, u32 buff_len)
{
	adena::game_server::CCrypt cc((irr::c8*)key);
	u32 loc = 0;
	while(loc < buff_len)
	{
		c8 temp[65536];
		u16 pack_size = 0;
		pack_size += (unsigned char)buff[loc++];
		pack_size += ((unsigned char)(buff[loc++]) * 256);
		memcpy(temp, buff + loc, pack_size - 2);
		loc += (pack_size - 2);
		cc.decrypt(temp, pack_size - 2);
		if((temp[0] & 0xff) < 0x10)
			printf("Packet size: %d Packet type: 0x0%x\n", pack_size, (temp[0] & 0xff));
		else
			printf("Packet size: %d Packet type: 0x%x\n", pack_size, (temp[0] & 0xff));
		hexdump(temp, pack_size - 2);
	}
	cout << "Loc " << loc << " Buff len " << buff_len << "\n";
}

int main()
{
	/*c8 buff[65536];
	u32 loc = 0;
	io::CFileSystem fs = io::CFileSystem();
	loc += addPack(&fs, buff + loc, "pack0");
	loc += addPack(&fs, buff + loc, "pack1");
	loc += addPack(&fs, buff + loc, "pack2");
	loc += addPack(&fs, buff + loc, "pack3");
	loc += addPack(&fs, buff + loc, "pack4");
	parseBuff(buff, loc);*/

	adena::login_server::CLoginServer* s = new adena::login_server::CLoginServer();
	adena::game_server::CGameServer* g = new adena::game_server::CGameServer();
	adena::login_server::CInProcessGameServerLink GameLink = adena::login_server::CInProcessGameServerLink(s);
	adena::game_server::CInProcessLoginServerLink LogLink = adena::game_server::CInProcessLoginServerLink(g, &GameLink, 0);
	s->GameServerLink = &GameLink;
	g->LoginServerLink = &LogLink;
	if(s->init("login.ini"))
	{
		s->start();
		if(g->init("game.ini"))
		{
			g->start();
			while(s->Running)
				irr::core::threads::sleep(1000);
		}
		else
		{
			cout << "Failed to init game server\n";
		}
	}else
	{
		cout << "Failed to init login server\n";
	}
	system("PAUSE");
	delete s;
	delete g;
	irr::net::Cleanup();
	return 0;
}

/*
char packet0[] = {0x87,0xb3,0xb3,0xb3,0x12,0x2a,0x7e,0x90,0x04,0x5f,0x5f,0x26,0x87,0xbb,0xef,0x09,0x9d,0xd8,0xd8,0xbd,0x1c,0x02,0x56,0x85,0x11,0x45,0x45,0x2b,0x8a,0x8d,0xd9,0x5e,0xca,0x85,0x8e,0x8d,0x2c,0x22,0x76,0x98,0x0c,0x5e,0x5e,0x3d,0x9c,0x98,0xcc,0x2e,0xba,0xea,0xea,0x99,0x38,0x31,0x65,0xe2,0x76,0xbc,0x43,0xbc,0xe2,0x78,0x76,0xee,0x6a,0x5f,0x5f,0x5f,0xfe,0x92,0xc6,0x41,0xd5,0xe3,0xe3,0xe3,0x42,0x5f,0x0b,0x8c,0x18,0x2c,0x2c,0x2c,0x8d,0xe1,0xb5,0x32,0xa6,0x93,0x93,0x93,0x32,0x5e,0x0a,0x8d,0x19,0x2c,0x2c,0x2c,0x8d,0xe1,0x60,0x5d,0x89,0xbc,0xbc,0xbc,0x1d,0x71,0x7d,0x64,0xb0,0x7a,0x85,0x7a,0xa4,0x03,0xa5,0x6e,0xab,0x9f,0x9f,0x9f,0x3e,0x02,0x56,0xd1,0x45,0x70,0x70,0x70,0xd1,0xbd,0xe9,0x6e,0xfa,0xcf,0xcf,0xcf,0x6e,0x02,0x56,0xd1,0x45,0x70,0x70,0x70,0xd1,0xbd,0xe9,0x6e,0xfa,0xcf,0xcf,0xcf,0x6e,0x02,0x56,0xd1,0x45,0x70,0x70,0x70,0xd1,0xbd,0xe9,0x6e,0xfa,0xe4,0x94,0x8a,0x3b,0x54,0x61,0xf9,0x7d,0x4c,0x2d,0x32,0x83,0xb0,0x9a,0x02,0x86,0x19,0x79,0x66,0xd7,0xf1,0xdf,0x47,0xc3,0xd1,0xb7,0xa8,0x19,0x75,0x21,0xa6,0x32,0x07,0x07,0x07,0xa6,0xec,0xde,0x46,0xc2,0xd3,0xb5,0xaa,0x1b,0x52,0x60,0xf8,0x7c,0x6a,0x0c,0x13,0xa2,0xce,0x9a,0x1d,0x89,0x74,0x2b,0x34,0x85,0xe9,0xbd,0x3a,0xae,0xf7,0xf5,0xf5,0x54,0x3b,0x75,0xf2,0x66,0x53,0x49,0x49,0xe8,0x85,0xcb,0x4c,0xd8,0xe9,0xf3,0xf3,0x52,0x3c,0x72,0xf5,0x61,0xbe,0xa6,0xa6,0x07,0x6b,0x3f,0xb8,0x2c,0x19,0x19,0x19,0xb8,0x33,0x7f,0xf8,0x6c,0xbc,0xa4,0xa4,0x05,0x8f,0xc3,0x44,0xd0,0x0d,0x15,0x15,0xb4,0xd8,0x8c,0x0b,0x9f,0x65,0x7c,0x7c,0xdd,0xb1,0xe5,0x62,0xf6,0xc7,0xc7,0xc7,0x66,0x09,0x5d,0xda,0x4e,0x7b,0x7b,0x7b,0xda,0xb6,0xe2,0x65,0xf1,0xc4,0xde,0x13,0xf2,0x9e,0xca,0x4d,0xd9,0xec,0x92,0x30,0xd1,0xbd,0xe9,0x6e,0xfa,0xbe,0xbe,0xbe,0x1f,0x72,0x26,0xa1,0x35,0x0a,0x0a,0x0a,0xab,0xc7,0x93,0x14,0x80,0xb5};
char packet1[] = {0xf9,0xc5,0xc5,0xc5,0x64,0x08,0x5c,0xdb,0x35,0x03,0x03,0x03,0xa2,0x88,0xdc,0x5b,0xb5,0xab,0xab,0xab,0x0a,0x6c,0x38,0xbf,0x51,0x21,0x21,0x21,0x80,0xf2,0xa6,0x21,0xcf,0xf3,0xf3,0xf3,0x52,0x78,0x2c,0xab,0x45,0x58,0x58,0x58,0xf9,0x9f,0xcb,0x4c,0xa2,0xd2,0xd2,0xd2,0x73,0x0a,0x5e,0xd9,0x37,0x0b,0x0b,0x0b,0xaa,0x80,0xd4,0x53,0xbd,0x80,0x80,0x80,0x21,0x47,0x13,0x94,0x7a,0x0a,0x0a,0x0a,0xab,0xde,0x8a,0x0d,0xe3,0xdf,0xdf,0xdf,0x7e,0x12,0x46,0xc1,0x2f,0x19,0x19,0x19,0xb8,0x92,0xc6,0x41,0xaf,0xb1,0xb1,0xb1,0x10,0x76,0x22,0xa5,0x4b,0x3b,0x3b,0x3b,0x9a,0xe8,0xbc,0x3b,0xd5,0xe9,0xe9,0xe9,0x48,0x62,0x36,0xb1,0x5f,0x42,0x42,0x42,0xe3,0x85,0xd1,0x56,0xb8,0xc8,0xc8,0xc8,0x69,0x10,0x44,0xc3,0x2d,0x11,0x11,0x11,0xb0,0x9a,0xce,0x49,0xa7,0x9a,0x9a,0x9a,0x3b,0x5d,0x09,0x8e,0x60,0x10,0x10,0x10,0xb1,0xc4,0x90,0x17,0xf9,0xc5,0xc5,0xc5,0x64,0x08,0x5c,0xdb,0x35,0x09,0x09,0x09,0xa8,0x82,0xd6,0x51,0xbf,0x9f,0x9f,0x9f,0x3e,0x58,0x0c,0x8b,0x65,0x15,0x15,0x15,0xb4,0xcd,0x99,0x1e,0xf0,0xcc,0xcc,0xcc,0x6d,0x47,0x13,0x94,0x7a,0x57,0x57,0x57,0xf6,0x90,0xc4,0x43,0xad,0xdd,0xdd,0xdd,0x7c,0x39,0x6d,0xea,0x04,0x38,0x38,0x38,0x99,0xb3,0xe7,0x60,0x8e,0xac,0xac,0xac,0x0d,0x6b,0x3f,0xb8,0x56,0x26,0x26,0x26,0x87,0xcc,0x98,0x1f,0xf1,0xcd,0xcd,0xcd,0x6c,0x01,0x55,0xd2,0x3c,0x18,0x18,0x18,0xb9,0x93,0xc7,0x40,0xae,0xbc,0xbc,0xbc,0x1d,0x7b,0x2f,0xa8,0x46,0x36,0x36,0x36,0x97,0xd8,0x8c,0x0b,0xe5,0xd9,0xd9,0xd9,0x78,0x52,0x06,0x81,0x6f,0x7d,0x7d,0x7d,0xdc,0xba,0xee,0x69,0x87,0xf7,0xf7,0xf7,0x56,0x2d,0x79,0xfe,0x10,0x2c,0x2c,0x2c,0x8d,0xa7,0xf3,0x74,0x9a,0xa2,0xa2,0xa2,0x03,0x65,0x31,0xb6,0x58,0x28,0x28,0x28,0x89,0xff,0xab,0x2c,0xc2,0xfe,0xfe,0xfe,0x5f,0x32,0x66,0xe1,0x0f,0x20,0x20,0x20,0x81,0xab,0xff,0x78,0x96,0xb5,0xb5,0xb5,0x14,0x72,0x26,0xa1,0x4f,0x3f,0x3f,0x3f,0x9e,0xea,0xbe,0x39,0xd7,0xeb,0xeb,0xeb,0x4a,0x60,0x34,0xb3,0x5d,0x72,0x72,0x72,0xd3,0xb5,0xe1,0x66,0x88,0xf8,0xf8,0xf8,0x59,0x10,0x44,0xc3,0x2d,0x11,0x11,0x11,0xb0,0x9a,0xce,0x49,0xa7,0x86,0x86,0x86,0x27,0x41,0x15,0x92,0x7c,0x0c,0x0c,0x0c,0xad,0xe9,0xbd,0x3a,0xd4,0xe8,0xe8,0xe8,0x49,0x27,0x73,0xf4,0x1a,0x33,0x33,0x33,0x92,0xb8,0xec,0x6b,0x85,0x9a,0x9a,0x9a,0x3b,0x5d,0x09,0x8e,0x60,0x10,0x10,0x10,0xb1,0xff,0xab,0x2c,0xc2,0xfe,0xfe,0xfe,0x5f,0x75,0x21,0xa6,0x48,0x5e,0x5e,0x5e,0xff,0x99,0xcd,0x4a,0xa4,0xd4,0xd4,0xd4,0x75,0x00,0x54,0xd3,0x3d,0x01,0x01,0x01,0xa0,0x8a,0xde,0x59,0xb7,0x8d,0x8d,0x8d,0x2c,0x4a,0x1e,0x99,0x77,0x07,0x07,0x07,0xa6,0xd0,0x84,0x03,0xed,0xd1,0xd1,0xd1,0x70,0x1e,0x4a,0xcd,0x23,0x33,0x33,0x33,0x92,0xb8,0xec,0x6b,0x85,0xa4,0xa4,0xa4,0x05,0x63,0x37,0xb0,0x5e,0x2e,0x2e,0x2e,0x8f,0xf4,0xa0,0x27,0xc9,0xf5,0xf5,0xf5,0x54,0x7e,0x2a,0xad,0x43,0x6d,0x6d,0x6d,0xcc,0xaa,0xfe,0x79,0x97,0xe7,0xe7,0xe7,0x46,0x06,0x52,0xd5,0x3b,0x07,0x07,0x07,0xa6,0x8c,0xd8,0x5f,0xb1,0x94,0x94,0x94,0x35,0x53,0x07,0x80,0x6e,0x1e,0x1e,0x1e,0xbf,0xf6,0xa2,0x25,0xcb,0xf7,0xf7,0xf7,0x56,0x39,0x6d,0xea,0x04,0x1e,0x1e,0x1e,0xbf,0x95,0xc1,0x46,0xa8,0xb6,0xb6,0xb6,0x17,0x71,0x25,0xa2,0x4c,0x3c,0x3c,0x3c,0x9d,0xeb,0xbf,0x38,0xd6,0xea,0xea,0xea,0x4b,0x61,0x35,0xb2,0x5c,0x45,0x45,0x45,0xe4,0x82,0xd6,0x51,0xbf,0xcf,0xcf,0xcf,0x6e,0x10,0x44,0xc3,0x2d,0x11,0x11,0x11,0xb0,0x9a,0xce,0x49,0xa7,0x9d,0x9d,0x9d,0x3c,0x5a,0x0e,0x89,0x67,0x17,0x17,0x17,0xb6,0xc1,0x95,0x12,0xfc,0xc0,0xc0,0xc0,0x61,0x0e,0x5a,0xdd,0x33,0x34,0x34,0x34,0x95,0xbf,0xeb,0x6c,0x82,0xaf,0xaf,0xaf,0x0e,0x68,0x3c,0xbb,0x55,0x25,0x25,0x25,0x84,0xf0,0xa4,0x23,0xcd,0xf1,0xf1,0xf1,0x50,0x7a,0x2e,0xa9,0x47,0x6e,0x6e,0x6e,0xcf,0xa9,0xfd,0x7a,0x94,0xe4,0xe4,0xe4,0x45,0x36,0x62,0xe5,0x0b,0x37,0x37,0x37,0x96,0xbc,0xe8,0x6f,0x81,0xb8,0xb8,0xb8,0x19,0x7f,0x2b,0xac,0x42,0x32,0x32,0x32,0x93,0xd5,0x81,0x06,0xe8,0xd4,0xd4,0xd4,0x75,0x1d,0x49,0xce,0x20,0x23,0x23,0x23,0x82,0xa8,0xfc,0x7b,0x95,0x84,0x84,0x84,0x25,0x43,0x17,0x90,0x7e,0x0e,0x0e,0x0e,0xaf,0xde,0x8a,0x0d,0xe3,0xdf,0xdf,0xdf,0x7e,0x54,0x00,0x87,0x69,0x72,0x72,0x72,0xd3,0xb5,0xe1,0x66,0x88,0xf8,0xf8,0xf8,0x59,0x21,0x75,0xf2,0x1c,0x20,0x20,0x20,0x81,0xab,0xff,0x78,0x96,0xaa,0xaa,0xaa,0x0b,0x6d,0x39,0xbe,0x50,0x20,0x20,0x20,0x81,0xf6,0xa2,0x25,0xcb,0xf7,0xf7,0xf7,0x56};
char packet2[] = {0x0a, 0x31, 0x31, 0x31, 0x90};
char packet3[] = {0x0b,0x33,0x33,0x33,0x92,0xbf,0xeb,0x08,0x10,0x4f,0x4f,0x21,0x80,0x8d,0xd9,0x2e,0x36,0x60,0x60,0x09,0xa8,0xbc,0xe8,0x6f,0x77,0x37,0x3c,0x3f,0x9e,0x90,0xc4,0x2a,0x32,0x6f,0x6f,0x0c,0xad,0xa9,0xfd,0x1f,0x07,0x58,0x58,0x2b,0x8a,0x83,0xd7,0x50,0x48,0x79,0x34,0x1e,0x39,0x55,0x01,0x86,0x9e,0xa4,0xa4,0xa4,0x05,0x68,0x3c,0xbb,0xa3,0x98,0x98,0x98,0x39,0x4c,0x18,0x9f,0x87,0xbc,0xbc,0xbc,0x1d,0x71,0x25,0xa2,0xba,0x80,0x80,0x80,0x21,0x4d,0x19,0x9e,0x86,0xbc,0xbc,0xbc,0x1d,0x71,0x25,0xfa,0xa2,0x98,0x98,0x98,0x39,0x55,0x01,0xc2,0x9a,0xa0,0xa0,0xa0,0x01,0x6d,0x39,0xbe,0xa6,0x9c,0x9c,0x9c,0x3d,0x50,0x04,0x83,0x9b,0xa1,0xa1,0xa1,0x00,0x6c,0x38,0xbf,0xa7,0x9d,0x9d,0x9d,0x3c,0x50,0x04,0x83,0x9b,0xa1,0xa1,0xa1,0x00,0x6c,0x38,0xbf,0xa7,0x9d,0x9d,0x9d,0x3c,0x50,0x04,0x83,0x9b,0xa1,0xa1,0xa1,0x00,0x6c,0x38,0xbf,0xa7,0x9d,0x9d,0x9d,0x3c,0x50,0x04,0x83,0x9b,0xa1,0xa1,0xa1,0x00,0x6c,0x38,0xbf,0xa7,0x9d,0x9d,0x9d,0x3c,0x50,0x04,0x83,0x9b,0xa1,0xa1,0xa1,0x00,0x1f,0x7d,0xda,0xd2,0xe8,0xe8,0xe8,0x49,0x25,0x71,0xf6,0xee,0xa4,0x92,0xb2,0x03,0x1d,0x7f,0xd8,0xd0,0xea,0xea,0xea,0x4b,0x27,0x73,0xf4,0xec,0xd6,0xd6,0xd6,0x77,0x1b,0x4f,0xc8,0xd0,0xea,0xea,0xea,0x4b,0x27,0x73,0xf4,0xec,0xd6,0xd6,0xd6,0x77,0x1b,0x4f,0xc8,0xd0,0xea,0xea,0xea,0x4b,0x27,0x73,0xf4,0xec,0xd6,0xd6,0xd6,0x77,0x1d,0x49,0xce,0xd6,0xec,0xec,0xec,0x4d,0x21,0x75,0xf2,0xea,0x79,0x78,0x78,0xd9,0x78,0x2d,0xaa,0xb2,0x88,0x88,0x88,0x29,0x45,0x11,0x96,0x8e,0xb4,0xb4,0xb4,0x15,0x79,0x2d,0xaa,0xb2,0x8c,0x8c,0x8c,0x2d,0x41,0x15,0x92,0x8a,0xb0,0xb0,0xb0,0x11,0x7d,0x29,0xae,0xb6,0x8c,0x8c,0xd4,0x35,0x59,0x0d,0x8a,0x92,0xa8,0xa8,0xfe,0x1f,0x73,0x27,0xa0,0xb8,0x9b,0x9b,0x9b,0x3a,0x57,0x03,0x84,0x9c,0xa6,0xa6,0xa6,0x07,0x6b,0x3f,0xb8,0xa0,0x9a,0xce,0xce,0x06,0x6a,0x50,0xd7,0xb6,0x8c,0xdc,0xdc,0x1c,0x70,0x54,0xd3,0xae,0x94,0xe6,0xe6,0x13,0x7f,0x4a,0xcd,0xbb,0x81,0xea,0xea,0x4b,0x27,0x09,0x85,0x9e,0xa4,0xc6,0xc6,0x0e,0x62,0x51,0xd6,0xad,0x97,0xff,0xff,0x3b,0x57,0x66,0xe1,0x8a,0xb0,0xd5,0xd5,0x74,0x18,0x47,0x8d,0xbf,0x03,0xf5,0xaf,0x11,0x6d,0x39,0xbe,0xa6,0x9c,0x9c,0x9c,0x3d,0x51,0x06,0x81,0x99,0xa3,0xd2,0xd2,0x73,0x1f,0x4a,0xcd,0xd5,0xef,0xef,0xef,0x4e,0x22,0x76,0xf1,0xe9,0xd3,0xd3,0xd3,0x72,0x1e,0x4a,0xcd,0xd5,0xef,0xef,0x3a,0x21,0x0d,0x59,0xde,0xc6,0xfc,0xfc,0xa4,0x9b,0xb7,0x1c,0x64,0x83,0xc6,0x0d,0xff,0x12,0x2f,0x7a,0xfd,0xe5,0xdf,0x8f,0x8f,0x2e,0x42,0x16,0x91,0x89,0xb3,0xb3,0xb3,0x12,0x7e,0x2a,0xad,0xb5,0x8f,0x8f,0x8f,0x2e,0x42,0x16,0x91,0x89,0xb3,0xb3,0xb3,0x12,0x7e,0x2a,0xad,0xb5,0x8f,0x8f,0x8f,0x2e,0x42,0x16,0x91,0x89,0xb3,0xb3,0xb3,0x12,0x7e,0x01,0xf6,0xf0,0xda,0xd9,0xb8,0x06,0x7a,0x2a,0xcc,0xcb,0xe1,0xbe,0xc0,0x7e,0x02,0xfc,0x1b,0x1c,0x36,0x7c,0x06,0xb8,0xc4,0xb7,0x56,0x51,0x7b,0x7b,0x7b,0xda,0xb6,0xe2,0x65,0x7d,0x47,0x61,0x07,0xb9,0xc5,0xb5,0x54,0x53,0x79,0x5c,0x3a,0x84,0xf8,0x8f,0x6e,0x69,0x43,0x43,0x43,0xe2,0x8e,0x12,0xca,0xcd,0xe7,0xe7,0xe7,0x46,0x2a,0x12,0x97,0x8f,0xb5,0xb6,0xac,0x0d,0x61,0x35,0xa8,0xb0,0x8a,0x8b,0x91,0x30,0x5c,0x0c,0x91,0x89,0xb3,0xb1,0xab,0x0a,0x66,0xd8,0x47,0x5f,0x65,0x65,0x65,0xc4,0xa8,0xfc,0x7b,0x63,0x59,0xbe,0xa6,0x07,0x6b,0xda,0x45,0x5d,0x67,0x81,0x99,0x38,0x54,0xe8,0x77,0x6f,0x55,0x55,0x55,0xf4,0x98,0x03,0x9d,0x85,0xbf,0xbf,0xbf,0x1e,0x72,0x22,0xa5,0xbd,0x87,0x84,0x84,0x25,0x49,0x1d,0x9a,0x82,0xb8,0xb8,0xb8,0x19,0x75,0x21,0xbc,0x69,0x13,0x13,0x13,0xb2,0xde,0x8a,0x73,0xc9,0xb3,0xb3,0xb3,0x12,0x7e,0x5b,0xdc,0xc4,0xfe,0xfe,0xfe,0x5f,0x33,0x6d,0xea,0xf2,0xc8,0xc8,0xc8,0x69,0x05,0x51};
char packet4[] = {0xaa,0xd7,0xd7,0xb3,0x12,0x1b,0x4f,0xa6,0x19,0x44,0x44,0x34,0x95,0x95,0xc1,0x2f,0x90,0xd4,0xd4,0xd4,0x75,0x63,0x3c,0xb8,0x07,0x3b,0x3b,0x30,0xdc,0x9a,0x48,0xcf,0x70,0x4c,0x4c,0x4c,0xed,0x81,0xd5,0x53,0xec,0xd0,0xd0,0xd1,0x70,0x1c,0x48,0xd6,0x69,0x55,0x55,0x54,0xf5,0x99,0xcd,0x2c,0x27,0x1b,0x1b,0xf5,0xf4,0x98,0xcc,0xdb,0x96,0x55,0xaa,0xaa,0x0b,0x67,0x33,0xb4,0x0b,0x6f,0x2f,0x2f,0x8e,0xe2,0xb6,0x31,0x8e,0xf6,0xb6,0xb6,0x17,0x7b,0x2f,0xa8,0x17,0x2b,0x2b,0x2b,0x8a,0xe6,0xb2,0x34,0x8b,0xb7,0xb7,0xb7,0x16,0x7a,0x2e,0xa9,0x16,0x2a,0x2a,0x0f,0xae,0xc2,0x96,0x04,0xbb,0x87,0x87,0x9e,0x3f,0x53,0x07,0xa8,0x17,0x2b,0x2b,0x33,0x92,0xfe,0xaa,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0xb9,0x18,0x74,0x20,0xa7,0x18,0x24,0x24,0x24,0x85,0xe9,0xbd,0x3a,0x85,0xb9,0xb9,0x83,0x24,0x48,0x1c,0x9b,0x24,0x18,0x18,0x18,0xb9,0xd5,0x81,0x06,0xb9,0x85,0x85,0x85,0x24,0x48,0x1c,0x9b,0x24,0x18,0x18,0x18,0xb9,0xd5,0x81,0x06,0xb9,0x85,0x85,0x85,0x24,0x48,0x1c,0x9b,0x24,0x18,0x18,0x18,0xb9,0xd5,0x81,0x06,0xb9,0x85,0x85,0x85,0x24,0x48,0x1c,0x9b,0x24,0x18,0x18,0x18,0xb9,0xd5,0x81,0x06,0xb9,0x85,0x85};
char packet5[] = {0x14,0xd8,0x72,0x72,0xc3,0x07,0xed,0x65,0x67,0x59,0x59,0x59,0xf8,0x92,0x69,0xee,0xec,0x4a,0xd7,0xd7,0x76,0x51,0xf7,0x8f,0x72,0x24,0x92,0x92,0x33,0x5f,0x0b,0x8c,0x8e,0xf0,0xf6,0xf6,0x57,0xa2,0xf7,0x70,0x72,0x8e,0x8e,0x8e,0x2f,0xc4,0x90,0x17,0x15,0xe9,0xe9,0xe9,0x48,0xa3,0xf7,0x70,0x72,0x8e,0x8e,0x8e,0x2f,0xc4,0x90,0x17,0x15,0xe9,0xe9,0xe9,0x48,0xa3,0xf7,0x70,0x72,0xd6,0x4f,0xd6,0xee,0x1b,0xd6,0xa0,0x9d,0xbc,0x31,0x2d,0xe4,0xfd,0x3c,0x4c,0x71,0x4f,0x4f,0x4f,0xee,0x82,0xd6,0x45,0x07,0x39,0x39,0x39,0x98,0xf4,0xa0,0x1e,0x5c,0x62,0x62,0x62,0xc3,0xaf,0xfb,0x7c,0x7e,0x40,0x40,0x40,0xe1,0x8c,0xd8,0x5f,0x5d,0x63,0x63,0x63,0xc2,0xae,0xfa,0x7d,0x7f,0x41,0x41,0x41,0xe0,0x8c,0xd8,0x5f,0x5d,0x63,0x63,0x63,0xc2,0xae,0xfa,0x7d,0x7f,0x41,0x41,0x41,0xe0,0x8c,0xd8,0x5f,0x5d,0x63,0x63,0x63,0xc2,0xae,0xfa,0x7d,0x7f,0x41,0x41,0x41,0xe0,0x8c,0xd8,0x5f,0x5d,0x63,0x63,0x63,0xc2,0xae,0xfa,0x7d,0x7f,0x41,0x41,0x41};
char packet6[] = {0xa0};*/