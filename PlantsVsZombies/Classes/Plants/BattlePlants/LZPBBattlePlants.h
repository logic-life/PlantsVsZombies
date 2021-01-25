/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2020.2.17
 *Email: 2117610943@qq.com
 */

#pragma once
#include "../LZPPlants.h"
#include "Scenes/SelectPlantsScene/LZSSSpriteLayer.h"

class Zombies;

class BattlePlants :public Plants
{
protected:
    virtual void hurtZombies(Zombies* zombie);
    virtual void plantExplode() {};
    virtual void plantAttack() {};
    virtual bool getZombieIsInExplodeRange(Zombies* zombie) { return false; };

CC_CONSTRUCTOR_ACCESS:
    BattlePlants();
    ~BattlePlants();
};