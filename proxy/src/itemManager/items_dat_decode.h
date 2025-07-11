#pragma once
#include <fstream>
#include <string>
#include <iostream>
#ifndef __linux__
#include <conio.h>
#endif
#include "../utils/json.hpp"
#include <iomanip>
#include "../utils/print/print.h"
//#include "world serialize/World.h"
void color(int y) {
	HANDLE h = GetStdHandle(STD_OUTPUT_HANDLE);

	SetConsoleTextAttribute(h, y);
}
using namespace std;
string to_lower(string s) {
	for (char& c : s)
		c = tolower(c);
	return s;
}
string to_upper(string s) {
	for (char& c : s)
		c = toupper(c);
	return s;
}
inline string getStrLower(string txt)
{
	string ret;
	for (char c : txt) ret += tolower(c);
	return ret;
}
struct types {
	float hair, shirt, pants = 0.f; //cloth_hair & cloth_shirt & cloth_pants
	float shoe, face, hand = 0.f; //cloth_feet & cloth_face & cloth_hand
	float back, mask, neck = 0.f;//cloth_back cloth_mask cloth_necklace
	float ances = 0; //cloth_ances
	float hat = 0.f;
};
using json = nlohmann::json;//items.dat decoderis
struct item {
std::string unknownTexturePath = "";
	int itemID = 0;
	char itemProps1 = 0;
	char itemProps2 = 0;
	char actionType = 0;
	char itemKind = 0;
	int val1;
	unsigned char itemCategory = 0;
	char hitSoundType = 0;
	std::string name = "";
	std::string description = "";
	std::string texture = "";
	int textureHash = 0;
	char visualEffect = 0;
	int ingredientType;
	int ingredient;
	char textureX = 0;
	char textureY = 0;
	char spreadType = 0;
	char isStripeyWallpaper = 0;
	char collisionType = 0;
	unsigned char breakHits = 0;
	int restoreTime = 0;
	char clothingType = 0;
	int16_t rarity = 0;
	unsigned char maxAmount = 0;
	std::string extraFile = "";
	int extraFileHash = 0;
	int IOSextraFileHash = 0;
	int audioVolume = 0;
	std::string petName = "";
	std::string petPrefix = "";
	std::string petSuffix = "";
	std::string petAbility = "";
	char seedBase = 0;
	char seedOverlay = 0;
	char treeBase = 0;
	char treeLeaves = 0;
	int16_t ing1 = 0;
	int16_t ing2 = 0;
	int seedColor = 0;
	int seedOverlayColor = 0;
	int growTime = 0;
	short val2;
	short isRayman = 0;
	std::string extraOptions = "";
	std::string texture2 = "";
	std::string extraOptions2 = "";
	std::string punchOptions = "";
	std::string extraFieldUnk_4 = "";
	short value = 0;
	short value2 = 0;
	short unkValueShort1 = 0;
	short unkValueShort2 = 0;
	int newValue = 0;
	char newValue1 = 0;
	char newValue2 = 0;
	char newValue3 = 0;
	char newValue4 = 0;
	char newValue5 = 0;
	char newValue6 = 0;
	char newValue7 = 0;
	char newValue8 = 0;
	char newValue9 = 0;
	int newInt1 = 0;
	int newInt2 = 0;
	bool canPlayerSit = false;
	int sitPlayerOffsetX = 0;
	int sitPlayerOffsetY = 0;
	int sitOverlayX = 0;
	int sitOverlayY = 0;
	int sitOverlayOffsetX = 0;
	int sitOverlayOffsetY = 0;
	std::string sitOverlayTexture = "";
};
std::unordered_map<uint32_t, item> item_map;
item* items = NULL;
int itemCount = 0;
vector<item> index;
int theid = 0;
inline item getItemDef(const int id)
{
	try {
		if (id < index.size() && id > 0 && id <= theid) {
			if (id == 1259 || id == 1261 || id == 1263 || id == 1265 || id == 1267 || id == 1269 || id == 1271 || id == 4309 || id == 4311 || id == 4313 || id == 4315 || id == 4317 || id == 4319) {
				index.at(id).name = "Surgical Tool Tree";
			}
			return index.at(id);
		}
	}
	catch (const std::out_of_range& e) {
		std::cout << e.what() << std::endl;
	}
	return index.at(0);
}

void decitem(string path) {//cia decoderis
	color(2);
	//print::set_text("Decoding Items.dat\n", LightGreen);
	std::ifstream file(path, std::ios::binary | std::ios::ate);
	int size = file.tellg();
	if (size == -1) {
		print::set_info("[ITEMS]: "," Didn't find items.dat. Is it at right place?\n", Red);
		exit(-1);
	}
	char* data = new char[size];
	file.seekg(0, std::ios::beg);
	if (file.read((char*)(data), size));
	else {
		print::set_info("[ITEMS]: "," Something gone wrong while decoding .dat file!\n", Red);
		exit(-1);
	}
	string secret = "PBG892FXX982ABC*";
	int memPos = 0;
	int itemsdatVer = 0;
	memcpy(&itemsdatVer, data + memPos, 2);
	memPos += 2;
	memcpy(&itemCount, data + memPos, 4);
	memPos += 4;
	items = new item[itemCount];
	if (itemsdatVer > 18) {
		print::set_info("[ITEMS]: "," Unsupported items.dat! Version:\n", Red);
		exit(-1);
	}
	for (int i = 0; i < itemCount; i++) {
		item item;
		{
			memcpy(&item.itemID, data + memPos, 4);
			memPos += 4;
		}
		{
			item.itemProps1 = data[memPos];
			memPos += 1;
		}
		{
			item.itemProps2 = data[memPos];
			memPos += 1;
		}
		{
			//item.itemCategory = data[memPos];
			item.actionType = data[memPos];
			memPos += 1;
		}
		{
			item.hitSoundType = data[memPos];
			memPos += 1;
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.name += data[memPos] ^ (secret[(j + item.itemID) % secret.length()]);
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.texture += data[memPos];
				memPos++;
			}
		}
		memcpy(&item.textureHash, data + memPos, 4);
		memPos += 4;
		item.itemKind = data[memPos];
		memPos += 1;
		memcpy(&item.val1, data + memPos, 4);
		memPos += 4;
		item.textureX = data[memPos];
		memPos += 1;
		item.textureY = data[memPos];
		memPos += 1;
		item.spreadType = data[memPos];
		memPos += 1;
		item.isStripeyWallpaper = data[memPos];
		memPos += 1;
		item.collisionType = data[memPos];
		memPos += 1;
		item.breakHits = data[memPos];
		memPos += 1;
		memcpy(&item.restoreTime, data + memPos, 4);
		memPos += 4;
		item.clothingType = data[memPos];
		memPos += 1;
		memcpy(&item.rarity, data + memPos, 2);
		memPos += 2;
		item.maxAmount = data[memPos];
		memPos += 1;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.extraFile += data[memPos];
				memPos++;
			}
		}
		memcpy(&item.extraFileHash, data + memPos, 4);
		memPos += 4;
		memcpy(&item.audioVolume, data + memPos, 4);
		memPos += 4;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.petName += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.petPrefix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.petSuffix += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.petAbility += data[memPos];
				memPos++;
			}
		}
		{
			item.seedBase = data[memPos];
			memPos += 1;
		}
		{
			item.seedOverlay = data[memPos];
			memPos += 1;
		}
		{
			item.treeBase = data[memPos];
			memPos += 1;
		}
		{
			item.treeLeaves = data[memPos];
			memPos += 1;
		}
		{
			memcpy(&item.seedColor, data + memPos, 4);
			memPos += 4;
		}
		{
			memcpy(&item.seedOverlayColor, data + memPos, 4);
			memPos += 4;
		}
		memPos += 4; // deleted ingredients
		{
			memcpy(&item.growTime, data + memPos, 4);
			memPos += 4;
		}
		memcpy(&item.val2, data + memPos, 2);
		memPos += 2;
		memcpy(&item.isRayman, data + memPos, 2);
		memPos += 2;
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.extraOptions += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.texture2 += data[memPos];
				memPos++;
			}
		}
		{
			int16_t strLen = *(int16_t*)&data[memPos];
			memPos += 2;
			for (int j = 0; j < strLen; j++) {
				item.extraOptions2 += data[memPos];
				memPos++;
			}
		}
		//memPos += 80;
		{
			int16_t extraField4Length = 0;
			memcpy(&extraField4Length, data + memPos, 2);
			memPos += 2;
			for (int j = 0; j < extraField4Length; j++) {
				item.extraFieldUnk_4 += data[memPos];
				memPos++;
			}
			memPos += 4;
			memcpy(&item.value, data + memPos, 2);
			memPos += 2;
			memcpy(&item.value2, data + memPos, 2);
			memPos += 2;
			memcpy(&item.unkValueShort1, data + memPos, 2);
			//if (unkValueShort1 & 4) cout << itemID << ":Transmute" << endl;
			memPos += 2 + (16 - item.value);
			memcpy(&item.unkValueShort2, data + memPos, 2);
			memPos += 2;
		}
		memPos += 50;
		if (itemsdatVer >= 11) {
			{
				int16_t strLen = *(int16_t*)&data[memPos];
				memPos += 2;
				for (int j = 0; j < strLen; j++) {
					item.punchOptions += data[memPos];
					memPos++;
				}
			}
		}
		if (itemsdatVer >= 12) {
			memcpy(&item.newValue, data + memPos, 4);
			memPos += 4;
			item.newValue1 = data[memPos];
			memPos++;
			item.newValue2 = data[memPos];
			memPos++;
			item.newValue3 = data[memPos];
			memPos++;
			item.newValue4 = data[memPos];
			memPos++;
			item.newValue5 = data[memPos];
			memPos++;
			item.newValue6 = data[memPos];
			memPos++;
			item.newValue7 = data[memPos];
			memPos++;
			item.newValue8 = data[memPos];
			memPos++;
			item.newValue9 = data[memPos];
			memPos++;
		}
		if (itemsdatVer >= 13) {
			memcpy(&item.newInt1, data + memPos, 4);
			memPos += 4;
		}
		if (itemsdatVer >= 14) {
			memcpy(&item.newInt2, data + memPos, 4);
			memPos += 4;
		}
		if (itemsdatVer >= 15) memPos += *(uint16_t*)&data[memPos + 25] + 27;
		if (itemsdatVer >= 16) memPos += *(uint16_t*)&data[memPos] + 2;
		if (itemsdatVer >= 17) memPos += 4;
		if (itemsdatVer >= 18) memPos += 4;
		if (i != item.itemID) {
			print::set_info("[ITEMS]: "," Unordered item! Something gone wrong?\n", Red);
			exit(-1);
		}
		index.push_back(item);
		item_map[i] = item;
		items[i] = item;
		theid = item.itemID;
	}
	print::set_info("[ITEMS]: "," Decoded Items.dat\n", LightBlue);
}
