/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2020.1.28
 *Email: 2117610943@qq.com
 */

#include "LZSGGameScene.h"
#include "LZSGBackgroundLayer.h"
#include "LZSGButtonLayer.h"
#include "LZSGInformationLayer.h"
#include "LZSGControlLayer.h"
#include "LZSGAnimationLayer.h"
#include "LZSGPauseQuitLayer.h"
#include "LZSGPauseLayer.h"
#include "LZSGGameTimerLayer.h"
#include "LZSGDefine.h"
#include "LZSGData.h"

#include "Based/LZBCar.h"
#include "Based/LZBCoin.h"
#include "Based/LZBPlayMusic.h"
#include "Based/LZBUserData.h"
#include "Based/LZBMouseEventControl.h"

#include "Plants/DefensePlants/LZPDSunFlower.h"
#include "Plants/EmissionPlants/Bullet/LZPEBBullet.h"
#include "Zombies/LZZZombies.h"

bool GameScene::isRunGameScene = false;

GameScene::GameScene() :
  _global(Global::getInstance())
, _director(Director::getInstance())
{
}

GameScene::~GameScene()
{
	DELETE_MAP_GROUP(PlantsGroup);
	DELETE_LIST_GROUP(ZombiesGroup);
	DELETE_LIST_GROUP(SunsGroup);
	DELETE_LIST_GROUP(BulletGroup);
	DELETE_LIST_GROUP(CarsGroup);
	DELETE_LIST_GROUP(CoinsGroup);

	Zombies::setZombiesNumbers(0);
	Zombies::zombiesWinOrLoseInit();

	_director->getEventDispatcher()->
		removeCustomEventListeners(GLViewImpl::EVENT_WINDOW_UNFOCUSED);

	isRunGameScene = false;
}

Scene* GameScene::createScene()
{
	return GameScene::create();
}

bool GameScene::init()
{
	if (!Scene::init())return false; 

	showGameLayer();
	pauseGame();

	isRunGameScene = true;
	return true;
}

void GameScene::showGameLayer()
{
	controlPlayMusic();
	backgroundLayer();   // ������
	informationLayer();  // ��Ϣ��
	buttonLayer();       // ��ť��
	controlLayer();      // ���Ʋ�
	animationLayer();    // ������
	goodsLayer();        // ��Ʒ��
	gameTimerLayer();    // ʱ���
}

void GameScene::controlPlayMusic()
{
	switch (_global->userInformation->getCurrentPlayLevels())
	{
	case 35: PlayMusic::changeBgMusic("mainmusic.mo3.2", true);                      break;
	case 36: PlayMusic::changeBgMusic("mainmusic.mo3.3", true);                      break;
	case 37: PlayMusic::changeBgMusic("mainmusic.mo3.7", true);                      break;
	case 50: PlayMusic::changeBgMusic("mainmusic.mo3.10", true);                     break;
	case 51: PlayMusic::changeBgMusic("mainmusic.mo3.11", true);                     break;
	case 52: PlayMusic::changeBgMusic("mainmusic.mo3.12", true);                     break;
	default: PlayMusic::changeBgMusic("bgMusic" + to_string(rand() % 11), true);     break;
	}
}

void GameScene::backgroundLayer()
{
	backgroundLayerInformation = GSBackgroundLayer::create();
	backgroundLayerInformation->addLayer(this, 0, "backgroundLayer");
}

void GameScene::controlLayer()
{
	controlLayerInformation = GSControlLayer::create();
	controlLayerInformation->addLayer(this, 1, "controlLayer");
}

void GameScene::animationLayer()
{
	animationLayerInformation = GSAnimationLayer::create(this);
	animationLayerInformation->addLayer(this, 2, "animationLayer");
}

void GameScene::informationLayer()
{
	informationLayerInformation = GSInformationLayer::create();
	informationLayerInformation->addLayer(this, 3, "informationLayer");
}

void GameScene::buttonLayer()
{
	buttonLayerInformation = GSButtonLayer::create();
	buttonLayerInformation->addLayer(this, 4, "buttonLayer");
}

void GameScene::goodsLayer()
{
	goodsLayerInformation = Layer::create();
	this->addChild(goodsLayerInformation, 5, "goodsLayer");
	MouseEventControl::goodsRecovery(goodsLayerInformation);
}

void GameScene::gameTimerLayer()
{
	this->addChild(GSGameTimerLayer::create(), 6, "gameTimerLayer");
}

void GameScene::pauseGame()
{
	_director->getEventDispatcher()->addCustomEventListener(
		GLViewImpl::EVENT_WINDOW_UNFOCUSED, [&](EventCustom* evt)
		{
			if (!GSPauseQuitLayer::getPauseNumbers())
			{
				PlayMusic::playMusic("pause");
				GSPauseQuitLayer::pauseLayer();
				_director->getRunningScene()->addChild(GSPauseLayer::addLayer(), 10);
			}
		});
}
