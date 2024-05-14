#pragma once
#include <iostream>
#include <Windows.h>
#include "Vector.h"
#include <vector>


struct Table {
	std::uint64_t contentsBase;
	int count;
};


struct World
{
    char pad_0000[40];
    unsigned __int64 EngineDx;

    char pad_0001[392];
    unsigned __int64 Camera;

    char pad_0002[2624];
    float Grass;

    char pad_0003[508];
    unsigned __int64 BulletTable;//unicorns
    unsigned int BulletTableCount;

    char pad_0004[316];
    unsigned __int64 NearTable;//players, animals, vehicles
    unsigned int NearTableCount;

    char pad_0005[316];
    unsigned __int64 FarTable;//players, animals, vehicles
    unsigned int FarTableCount;

    char pad_0006[3956];
    unsigned __int64 SlowTable; //dead entities, helicrashes, policecars
    unsigned int SlowTableCount;

    char pad_0007[68];
    unsigned __int64 ItemTable;
    unsigned int ItemTableCount;

    char pad_0008[2292];
    unsigned __int64 PlayerEntityLink;

    char pad_0009[12];
    float EyeAccom;
};


struct VisualState {
    
};


struct RTTI 
{
    unsigned long signature;
    unsigned long offset;
    unsigned long cd_offset;
    unsigned long rva_type_descriptor;
};

struct type_descriptor
{
private:
    char pad0[20];
public:
    char type_name[128];

    const char* get_type_name() { return type_name; }

};

struct Camera
{   
    unsigned __int64 VirtualTable;


    CVector Right;
    CVector Up;
    CVector Forward;
    CVector Translation;

    char pad_0000[32];
    CVector ViewPort;

    char pad_0001[108];
    CVector Projection1;
    CVector Projection2;


    bool ScreenPosition(CVector vWorld, CVector& vScreen)
    {

        CVector vTemp = vWorld - Translation;

        vTemp = {
            vTemp.Dot(Right),
            vTemp.Dot(Up),
            vTemp.Dot(Forward)
        };

        if (vTemp.z <= 0.19f)
            return false;

        vScreen = {
            ViewPort.x * (1.0F + vTemp.x / Projection1.x / vTemp.z),
            ViewPort.y * (1.0F - vTemp.y / Projection2.y / vTemp.z),
            vTemp.z
        };

        return true;
    }
};



//shit 










//entity
//{
//uint64_t entity_camera_entry = 0x2168;
//uint64_t entity_future_visual_state = 0x88;
//uint64_t entity_renderer_visual_state = 0x130;
//uint64_t entity_future_entity_type = 0x38;
//uint64_t entity_renderer_entity_type = 0xE0;
//uint64_t entity_sort_object = 0x138;
//uint64_t entity_base = 0xa8;
//uint64_t entity_netowrkid = 0x57c;
//uint64_t entity_inventory = 0x450;
//uint64_t entity_inventory_hands = 0x1b0;
//uint64_t entity_position = 0x2c;//visualstate_position
//}
//
//entity_type
//{
//uint64_t entity_type_base = 0x88;
//uint64_t entity_type_name = 0x68;
//uint64_t entity_type_model = 0x78;
//uint64_t entity_type_config_name = 0x98;
//uint64_t entity_type_clean_name = 0x450;
//}










//struct SLocalPlayer
//{
//    char padding01[464];
//    struct SVisualState
//    {
//        char padding01[44];
//        Vector3 vPosition;
//    }*pVisualState;
//};

//In order to get the position of a player, you need to 1st read their VisualState which is at 0x128 in an Entity, then the BasePosition of the player is 0x2C inside VisualState.
//1D0 VisualState
//0x2C X
//0x30 Z
//0x34 Y

/* respawn player
void FUN_14054f000(void)

{
  undefined8 uVar1;
  char cVar2;

  cVar2 = FUN_140540310();
  if (cVar2 == '\0') {
	FUN_1409bdb00("Unable to respawn player.");
	return;
  }
  uVar1 = FUN_14067c300();
  FUN_14067e260(uVar1);
  FUN_1408365e0(DAT_14414cf70);
  return;
}
*/

/*
float FUN_140545bb0(void)

{
  float fVar1;

  fVar1 = (float)FUN_1407efbb0(DAT_14414c6b0,0x10);
  return fVar1 * 0.001;
}

*/

//std::string read_string(UINT_PTR String_address, SIZE_T size)
//{
//    if (size == 0 || !String_address || size > 1024) return _("");
//    std::unique_ptr<char[]> buffer(new char[size]);
//    driver.ReadArr(String_address, buffer.get(), size);
//    return std::string(buffer.get());
//}
//
//std::string ReadArmaString(uint64_t address)
//{
//    int Len = driver.Read<int>(address + GameAddress::LENGTH);
//    if (Len > 120) return "";
//    int length = driver.Read<int>(address + GameAddress::LENGTH);
//    std::string text = read_string(address + GameAddress::TEXT, length);
//    return text.c_str();
//}