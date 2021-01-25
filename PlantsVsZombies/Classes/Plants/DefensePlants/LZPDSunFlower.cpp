/**
 *Copyright (c) 2020 LZ.All Right Reserved
 *Author : LZ
 *Date: 2020.2.6
 *Email: 2117610943@qq.com
 */

#include "LZPDSunFlower.h"

#include "Scenes/GameScene/LZSGData.h"
#include "Scenes/GameScene/LZSGBackgroundLayer.h"
#include "Scenes/GameScene/LZSGInformationLayer.h"
#include "Scenes/SelectPlantsScene/LZSSSpriteLayer.h"

#include "Based/LZBGameType.h"
#include "Based/LZBMouseEventControl.h"
#include "Based/LZBPlayMusic.h"

int SunFlower::_sunTag = -1;

SunFlower::SunFlower(Node* node) :
  _sun(nullptr)
, _sunShowTime(Vec2(4 + rand() % 3, 16 + rand() % 5))
, _sunShowTime1(_sunShowTime)
{
	_node = node;
	_coolDownTime = 7.5f;
	_plantImage = nullptr;
	_plantsType = PlantsType::SunFlower;
	_healthPoint = 300;
}

SunFlower::~SunFlower()
{
}

SunFlower* SunFlower::create(Node* node)
{
	SunFlower* sunFlower = new (std::nothrow) SunFlower(node);
	if (sunFlower && sunFlower->init())
	{
		sunFlower->autorelease();
		return sunFlower;
	}
	CC_SAFE_DELETE(sunFlower);
	return nullptr;
}

void SunFlower::stopSun()
{
	for (auto sun : SunsGroup)
	{
		sunRecovery(sun);
	}
}

void SunFlower::createRandomSuns()
{
	_node->runAction(RepeatForever::create(Sequence::create(DelayTime::create(5),
		CallFunc::create([this]()
			{
				auto sun = new Sun(goodsLayerInformation);
				sun->setSunTag(++_sunTag);
				sun->setPosition(Vec2(rand() % 1100 + 500, 1150));
				sun->createSuns();

				auto temporary = sun->getSunTag();
				sun->getSun()->setScale(1.2f);
				sun->getSun()->runAction(Sequence::create(
					MoveBy::create(5.0f, Vec2(0, -rand() % 500 - 500)), 
					DelayTime::create(14), FadeOut::create(0.5f), 
					CallFunc::create([sun]() 
						{
							sun->getSun()->setVisible(false); 
						}), nullptr));
				
				SunsGroup.push_back(sun);
			}), DelayTime::create(20), nullptr)));
}

Sprite* SunFlower::createPlantImage()
{
	imageInit("SunFlower", INIT);
	_plantImage->setScale(1.5f);
	return _plantImage;
}

void SunFlower::createPlantAnimation()
{
	_plantAnimation = plantInit("SunFlower_2", "SunFlower_Normal");
	_plantAnimation->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_plantAnimation->setScale(0.8f);
	_plantAnimation->setSkin("SunFlower_Normal");
	_node->addChild(_plantAnimation);

	// 影子
	setPlantShadow(2.3f);

	// 泥土飞溅动画
	setPlantSoilSplashAnimation(1.0f);
	
	createListener();
}

void SunFlower::createListener()
{
	if (_global->userInformation->getIsReadFileData())
	{
		if (_sunShowTime1.x <= 0)
		{
			_plantAnimation->runAction(Sequence::create(DelayTime::create(_sunShowTime1.y),
				CallFunc::create([&]()
					{
						playAnimation();
					}), nullptr));
		}
		else
		{
			_plantAnimation->runAction(Sequence::create(DelayTime::create(_sunShowTime1.x),
				CallFunc::create([&]() { createSuns(); }), DelayTime::create(0.5f),
				CallFunc::create([=]() { _plantAnimation->setSkin("SunFlower_Normal"); }), DelayTime::create(_sunShowTime1.y),
				CallFunc::create([&]()
					{
						playAnimation();
					}), nullptr));
		}
	}
	else
	{
		playAnimation();
	}

	calculateSunShowTime();
}

void SunFlower::playAnimation()
{
	auto delaytime = DelayTime::create(_sunShowTime.x);
	auto delaytime1 = DelayTime::create(0.5f);
	auto delaytime2 = DelayTime::create(_sunShowTime.y);
	auto callfunc = CallFunc::create([&]() { createSuns(); });
	auto callfunc1 = CallFunc::create([=]() { _plantAnimation->setSkin("SunFlower_Normal"); /* 设置皮肤 */});

	_plantAnimation->runAction(RepeatForever::create(Sequence::create(delaytime, callfunc, delaytime1, callfunc1, delaytime2, nullptr)));
}

void SunFlower::calculateSunShowTime()
{
	_plantAnimation->runAction(RepeatForever::create(Sequence::create(DelayTime::create(0.05f),
		CallFunc::create([this]()
			{
				if (_sunShowTime1.x > 0)_sunShowTime1.x -= 0.05f;
				else if (_sunShowTime1.y > 0)_sunShowTime1.y -= 0.05f;
				else _sunShowTime1 = _sunShowTime;
			}), nullptr)));
}

void SunFlower::setSunShowTime(const Vec2 time)
{
	_sunShowTime1 = time;
}

Vec2 SunFlower::getSunShowTime() const
{
	return _sunShowTime1;
}

void SunFlower::sunRecovery(Sun* sun)
{
	auto temporary = sun;
	sun->setEnable(false);
	sun->getSun()->stopAllActions();
	sun->getSun()->runAction(Sequence::create(EaseSineOut::create(MoveTo::create(sun->calculateMoveTime(), Vec2(290, 1010))),
		Spawn::create(ScaleTo::create(0.2f, 0.3f), FadeOut::create(0.2f), nullptr),
		CallFunc::create([=]()
			{
				Global::getInstance()->userInformation->setSunNumbers(Global::getInstance()->userInformation->getSunNumbers() + 50);
				informationLayerInformation->updateSunNumbers();
				backgroundLayerInformation->gameType->updateRequirementNumbers("阳光数量增加");
			}), DelayTime::create(5.0f),
		CallFunc::create([temporary]()
			{
				temporary->getSun()->setVisible(false);
			}), nullptr));
}

void SunFlower::createSuns()
{
	PlayMusic::playMusic(rand() % 2 == 0 ? "throw" : "throw2");
	_plantAnimation->setSkin("SunFlower_ProduceSun"); /* 设置皮肤 */

	_sun = new Sun(goodsLayerInformation);
	_sun->setSunTag(++_sunTag);
	_sun->setPosition(_plantAnimation->getPosition());
	_sun->createSuns();

	SunsGroup.push_back(_sun);
}

SkeletonAnimation* SunFlower::showPlantAnimationAndText()
{
	auto& lta = _global->userInformation->getGameText();
	SPSSpriteLayer::plantCardTextScrollView->setInnerContainerSize(Size(lta.find("SUNFLOWER_1")->second->position));

	_isLoop = true;
	_plantAnimation = plantInit("SunFlower_2", "SunFlower_Normal");
	_plantAnimation->setSkin("SunFlower_Normal");
	_plantAnimation->setAnchorPoint(Vec2::ANCHOR_MIDDLE);
	_plantAnimation->setScale(1.5f);
	_plantAnimation->setPosition(Vec2(200, 610));

	SPSSpriteLayer::createPlantsText(0, lta.find("SUNFLOWER_1")->second->text, Vec2(190, 910), lta.find("SUNFLOWER_1")->second->fontsize);
	SPSSpriteLayer::createPlantsText(1, lta.find("SUNFLOWER_2")->second->text, Vec2(360, 1000), lta.find("SUNFLOWER_2")->second->fontsize, Color3B::YELLOW, false);
	SPSSpriteLayer::createPlantsText(2, lta.find("SUNFLOWER_3")->second->text, Vec2(440, 1000), lta.find("SUNFLOWER_3")->second->fontsize, Color3B::RED, false);
	SPSSpriteLayer::createPlantsText(3, lta.find("SUNFLOWER_4")->second->text, Vec2(360, 830), lta.find("SUNFLOWER_4")->second->fontsize, Color3B::YELLOW, false);

	return _plantAnimation;
}

void SunFlower::cavePlantInformation(rapidjson::Value& object, rapidjson::Document::AllocatorType& allocator)
{
	object.AddMember("SunShowTime.X", _sunShowTime1.x, allocator);
	object.AddMember("SunShowTime.Y", _sunShowTime1.y, allocator);
}

void SunFlower::readPlantInforamtion(rapidjson::Document* levelDataDocument, char* key, int i)
{
	_sunShowTime1 = Vec2((*levelDataDocument)[key]["Plants"][to_string(i).c_str()]["SunShowTime.X"].GetFloat(),
		(*levelDataDocument)[key]["Plants"][to_string(i).c_str()]["SunShowTime.Y"].GetFloat());
}

Sun::Sun(Node* node) :
    _node(node)
,	_sunTag(0)
,   _sun(nullptr)
,   _isEnable(true)
,   _global(Global::getInstance())
{
}

void Sun::createSuns()
{
	_sun = SkeletonAnimation::createWithData(_global->userInformation->getAnimationData().find("Sun")->second);
	_sun->addAnimation(0, "Sun_rotate", true);
	_sun->setPosition(_position);
	_sun->runAction(Sequence::create(
		Spawn::create(ScaleTo::create(0.5f, 1.2f), JumpBy::create(0.6f, Vec2(rand() % 120 - 60, 0), rand() % 100 + 100, 1), nullptr), 
		DelayTime::create(14), FadeOut::create(0.5f), DelayTime::create(5.0f), CallFunc::create([this]() {_sun->setVisible(false); }), nullptr));
	_node->addChild(_sun);
}

void Sun::setSunTag(const int& sunTag)
{
	_sunTag = sunTag;
}

void Sun::setPosition(const Vec2& position)
{
	_position = position;
}

void Sun::setEnable(bool isUsed)
{
	_isEnable = isUsed;
}

int Sun::getSunTag() const
{
	return _sunTag;
}

bool Sun::getEnable() const
{
	return _isEnable;
}

void Sun::releaseSun() const
{
	_sun->removeFromParent();
}

float Sun::calculateMoveTime()
{
	auto distance = sqrt(pow(_sun->getPosition().x - 290, 2) + pow(_sun->getPosition().y - 1010, 2));
	return static_cast<float>(distance / (750 + distance));
}

SkeletonAnimation* Sun::getSun()
{
	return _sun;
}

void Sun::deleteSun(list<Sun*>::iterator& sun)
{
	if (!(*sun)->getSun()->isVisible())
	{
		(*sun)->releaseSun();
		delete* sun;
		*sun = nullptr;
		SunsGroup.erase(sun++);
	}
	else
	{
		++sun;
	}
}