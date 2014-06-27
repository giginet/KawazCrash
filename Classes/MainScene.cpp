//
//  MainScene.cpp
//  KawazCrash
//
//  Created by giginet on 6/19/14.
//
//

#include "MainScene.h"
#include "Entity.h"
#include <algorithm>

#include "CueSheet_0.h"
#include "test_acf.h"

#include <AudioToolbox/AudioSession.h>

USING_NS_CC;

const int HORIZONTAL_COUNT = 6;
const int VERTICAL_COUNT = 8;
const int VANISH_COUNT = 4;

/* ADX2 関連 */
namespace {
    /* ボイスプールへの設定 */
    const int	max_voice			= 8;		/* デフォルト設定でも8	*/
    const int	max_sampling_rate	= 48000*2;	/* ピッチ変更も加味		*/
    
    const float	pitch_change_value	= 200.0f;
    
    CriAtomExAcbHn			g_acb;			/* 波形データバンクのハンドル	*/
    CriAtomExVoicePoolHn	g_voice_pool;	/* ボイスプールのハンドル	 	*/
    CriAtomExPlayerHn		g_player;		/* 音声プレーヤのハンドル		*/
    CriAtomDbasId			g_dbas_id;		/* D-BASのID				*/
    
    /* 多重初期化を防ぐためのカウンタ  */
    CriUint32				g_initialize_count = 0;
    
    //---------------------------------------------
    /*---- ADX2関連 ----*/
    //---------------------------------------------
    /*---- 変数定義 ----*/
    /* ADX2 Atomライブラリ初期化カウント(多重初期化の回避) */
    /* キュー一覧 */
    CriAtomExCueId g_cue_list[] = {
        CRI_CUESHEET_0_BGM_INT,
    };
    const CriUint32 g_num_cue_items = sizeof(g_cue_list)/sizeof(CriAtomExCueId);
    
    /* プレイヤーの状態 */
    const std::string g_playback_status_description[] = {
        "--",
        "Prepare",
        "Playing",
        "Removed",
    };
    
    /* 関数宣言 */
    void adx2_initialize(void);
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    void adx2_interruption_listener(void *userData, UInt32 interruptionState);
#endif
    void adx2_finalize(void);
    void adx2_update(void);
    CriSint64 adx2_play_cue_by_id(CriUint32 cue_id);
    void adx2_stop(CriSint64 playback_id);
    void adx2_stop_all(void);
    void adx2_pause_all(CriBool sw);
    void adx2_get_resource_path(CriChar8 *full_path, const CriChar8 *loc_path);
    void *adx2_alloc(void *obj, CriUint32 size);
    void adx2_free(void *obj, void *ptr);
    void adx2_error_callback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray);
    const char* adx2_get_cue_name(CriAtomExCueId cue_id);
    std::string adx2_get_voice_num(void);
    std::string adx2_get_time(CriAtomExCueId cue_id);
    
    void adx2_initialize(void)
    {
        /* 初期化カウントアップ */
        g_initialize_count++;
        if (g_initialize_count > 1) {
            /* 多重初期化は許さず */
            return;
        }
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        AudioSessionInitialize(NULL, NULL, adx2_interruption_listener, NULL);
        UInt32 category = kAudioSessionCategory_AmbientSound;
        AudioSessionSetProperty(kAudioSessionProperty_AudioCategory, sizeof(category), &category);
        AudioSessionSetActive(true);
#endif
        criErr_SetCallback(&adx2_error_callback);
        
        criAtomEx_SetUserAllocator(&adx2_alloc, &adx2_free, NULL);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Initialize_PC(NULL, NULL, 0);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        criAtomEx_Initialize_IOS(NULL, NULL, 0);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        criAtomEx_Initialize_ANDROID(NULL, NULL, 0);
        
        /* ANDROIDの場合で必要な一手間。assetsフォルダへのアクセスを可能にする */
        /* まずはJniHelperでActivityのContextを取得 */
        cocos2d::JniMethodInfo methodInfo;
        cocos2d::JniHelper::getStaticMethodInfo(methodInfo,
                                                "org/cocos2dx/lib/Cocos2dxActivity",
                                                "getContext",
                                                "()Landroid/content/Context;");
        auto android_context_object = (jobject)methodInfo.env->CallStaticObjectMethod( methodInfo.classID, methodInfo.methodID );
        /* 有効化。assetsフォルダはCocosプロジェクトのResource相当なので、ほぼ必須と言って良い手順 */
        criFs_EnableAssetsAccess_ANDROID(cocos2d::JniHelper::getJavaVM(), android_context_object);
#endif
        
        g_dbas_id = criAtomDbas_Create(NULL, NULL, 0);
        
        CriAtomExStandardVoicePoolConfig vp_config;
        
        /* ボイスプールの設定。まずはデフォルト設定にして、その上で必要な値へ書き換えていく */
        criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vp_config);
        vp_config.num_voices = max_voice;
        vp_config.player_config.streaming_flag		= CRI_TRUE;
        vp_config.player_config.max_sampling_rate	= max_sampling_rate;
        
        /* 上で作った設定オブジェクトを渡して、ボイスプールを作成 */
        g_voice_pool = criAtomExVoicePool_AllocateStandardVoicePool(&vp_config, NULL, 0);
        
        CriChar8 acf_path[256], acb_path[256];
        adx2_get_resource_path(acf_path, "test.acf");
        adx2_get_resource_path(acb_path, "CueSheet_0.acb");
        criAtomEx_RegisterAcfFile(NULL, acf_path, NULL, 0);
        
        g_acb = criAtomExAcb_LoadAcbFile(NULL, acb_path, NULL, NULL, NULL, 0);
        
        /* Player作成にも設定は必要 */
        CriAtomExPlayerConfig pf_config;
        criAtomExPlayer_SetDefaultConfig(&pf_config);
        pf_config.max_path_strings	= 1;
        pf_config.max_path			= 256;
        g_player = criAtomExPlayer_Create(&pf_config, NULL, 0);
    }
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
    void adx2_interruption_listener(void *userData, UInt32 interruptionState)
    {
        switch (interruptionState) {
            case kAudioSessionBeginInterruption:
                criAtomEx_StopSound_IOS();
                break;
            case kAudioSessionEndInterruption:
                AudioSessionSetActive(true);
                criAtomEx_StartSound_IOS();
                break;
        }
    }
#endif
    
    void adx2_finalize(void)
    {
        /* 早すぎる終了処理をしないように。Atomを使ってるHelloWorldがまだあるかもしれない */
        g_initialize_count--;
        if (g_initialize_count > 0) {
            return;
        }
        
        criAtomExPlayer_Stop(g_player);
        criAtomExVoicePool_Free(g_voice_pool);
        criAtomExAcb_Release(g_acb);
        criAtomEx_UnregisterAcf();
        criAtomDbas_Destroy(g_dbas_id);
        
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        criAtomEx_Finalize_IOS();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
        criAtomEx_Finalize_ANDROID();
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        criAtomEx_Finalize_PC();
#endif
    }
    
    /* HelloWorldレイヤのupdateで呼ぶ */
    void adx2_update(void)
    {
        criAtomEx_ExecuteMain();
    }
    
    /* ID単位で音を鳴らす処理。鳴らした音のプレイバックIDが返る */
    CriSint64  adx2_play_cue_by_id(CriAtomExCueId cue_id)
    {
        criAtomExPlayer_SetCueId(g_player, g_acb, cue_id);
        
        CriSint64 playback_id =  criAtomExPlayer_Start(g_player);
        
        return playback_id;
    }
    
    /* プレイバックID単位で音を止める */
    void adx2_stop(CriSint64 playback_id)
    {
        criAtomExPlayback_Stop(playback_id);
    }
    
    /* プレイヤーごと音を止める */
    void adx2_stop_all(void)
    {
        criAtomExPlayer_Stop(g_player);
    }
    
    void adx2_get_resource_path(CriChar8 *full_path, const CriChar8 *loc_path)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        sprintf(full_path, "%s", CCFileUtils::sharedFileUtils()->fullPathForFilename(loc_path).c_str());
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        /* assetsへのアクセスパスは、必ずassets以下からの相対パスでファイルを指定する
         * パスの先頭に'/'文字が入ってはダメ */
        sprintf(full_path, "%s", loc_path);
#endif
    }
    
    void *adx2_alloc(void *obj, CriUint32 size)
    {
        return malloc(size);
    }
    
    void adx2_free(void *obj, void *ptr)
    {
        free(ptr);
    }
    
    void adx2_error_callback(const CriChar8 *errid, CriUint32 p1, CriUint32 p2, CriUint32 *parray)
    {
        const CriChar8 *errmsg;
        errmsg = criErr_ConvertIdToMessage(errid, p1, p2);
        CCLOG("%s\n", errmsg);
    }
    
    const char* adx2_get_cue_name(CriAtomExCueId cue_id)
    {
        return criAtomExAcb_GetCueNameById(g_acb, cue_id);
    }
    
    /* 現在使用中のボイス数を(文字列表現で)取得する */
    std::string adx2_get_voice_num(void)
    {
        CriSint32 current_vnum;
        criAtomExVoicePool_GetNumUsedVoices(g_voice_pool, &current_vnum, NULL);
        
        /* 整数 --> 文字列 に変換 */
        std::stringstream time_ss;
        time_ss << "[VoiceNum: " << current_vnum << "/" << max_voice << "]";
        return time_ss.str();
    }
    
    /* 現在プレイバック中の音声の再生時間を(文字列表現で)取得する */
    std::string adx2_get_time(CriAtomExCueId cue_id)
    {
        /* 整数 --> 文字列 に変換 */
        std::stringstream time_ss;
        time_ss << "[Time(msec): " << criAtomExPlayback_GetTime(cue_id) << "]";
        return time_ss.str();
    }
}

MainScene::MainScene() : _currentEntity(nullptr)
{
    adx2_initialize();
}

MainScene::~MainScene()
{
    adx2_finalize();

    CC_SAFE_RELEASE_NULL(_currentEntity);
}

Scene* MainScene::createScene()
{
    auto scene = Scene::create();
    auto main = MainScene::create();
    scene->addChild(main);
    return scene;
}

bool MainScene::init()
{
    if (!Layer::init()) {
        return false;
    }
    
    this->setStage(Node::create());
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        for (int y = 0; y < VERTICAL_COUNT; ++y) {
            auto entity = Entity::create();
            entity->setEntityPosition(Vec2(x, y));
            this->addEntity(entity);
        }
    }
    auto winSize = Director::getInstance()->getWinSize();
    auto leftMargin = winSize.width - Entity::getSize() * HORIZONTAL_COUNT;
    _stage->setPosition(Vec2(leftMargin / 2 + Entity::getSize() / 2.0, 50));
    this->addChild(_stage);
    
    auto listener = EventListenerTouchOneByOne::create();
    listener->onTouchBegan = [this](Touch* touch, Event* event) {
        auto position = touch->getLocation();
        auto entity = this->getEntityAt(position);
        this->setCurrentEntity(entity);
        return true;
    };
    listener->onTouchMoved = [this](Touch* touch, Event* event) {
        auto nextEntity = this->getEntityAt(touch->getLocation());
        if (_currentEntity != nullptr && nextEntity != nullptr && _currentEntity != nextEntity) {
            if (_currentEntity->isNormal() && nextEntity->isNormal()) {
                auto cp = _currentEntity->getEntityPosition();
                auto np = nextEntity->getEntityPosition();
                if (cp.y == np.y && cp.x + 1 == np.x) { // 右方向
                    this->swapEntities(_currentEntity, nextEntity);
                    this->setCurrentEntity(nullptr);
                }
                if (cp.y == np.y && cp.x - 1 == np.x) { // 左方向
                    this->swapEntities(_currentEntity, nextEntity);
                    this->setCurrentEntity(nullptr);
                }
                if (cp.x == np.x && cp.y + 1 == np.y) { // 上方向
                    this->swapEntities(_currentEntity, nextEntity);
                    this->setCurrentEntity(nullptr);
                }
                if (cp.x == np.x && cp.y - 1 == np.y) { // 下方向
                    this->swapEntities(_currentEntity, nextEntity);
                    this->setCurrentEntity(nullptr);
                }
            }
        }
    };
    listener->onTouchEnded = [this](Touch* touch, Event* event) {
        this->setCurrentEntity(nullptr);
    };
    listener->onTouchCancelled = [this](Touch* touch, Event* event) {
        this->setCurrentEntity(nullptr);
    };
    this->getEventDispatcher()->addEventListenerWithSceneGraphPriority(listener, this);
    this->scheduleUpdate();
    
    return true;
}

void MainScene::onEnterTransitionDidFinish()
{
    Layer::onEnterTransitionDidFinish();
    adx2_play_cue_by_id(CRI_CUESHEET_0_BGM_INT);
    adx2_play_cue_by_id(CRI_CUESHEET_0_CUENUM);
}

void MainScene::update(float dt)
{
    adx2_update();
    for (auto entity : _entities) {
        this->checkFall(entity);
    }
    this->spawnEntities();
    this->checkField();
}

Entity* MainScene::getEntityAt(int x, int y)
{
    for (auto entity : _entities) {
        if (entity->getEntityPosition().x == x && entity->getEntityPosition().y == y) {
            return entity;
        }
    }
    return nullptr;
}

Entity* MainScene::getEntityAt(cocos2d::Vec2 position)
{
    auto stagePoint = _stage->convertToNodeSpace(position);
    auto x = floor((stagePoint.x + Entity::getSize() / 2.0) / Entity::getSize());
    auto y = floor((stagePoint.y + Entity::getSize() / 2.0) / Entity::getSize());
    return this->getEntityAt(x, y);
}

void MainScene::addEntity(Entity *entity)
{
    _entities.pushBack(entity);
    _stage->addChild(entity);
    entity->adjustPosition();
}

bool MainScene::moveEntity(Entity *entity, cocos2d::Vec2 entityPosition)
{
    entity->setEntityPosition(entityPosition);
    entity->adjustPosition();
    return true;
}

bool MainScene::swapEntities(Entity *entity0, Entity *entity1)
{
    const auto duration = 0.2;
    
    auto position0 = entity0->getPosition();
    auto position1 = entity1->getPosition();
    
    auto currentPosition0 = entity0->getEntityPosition();
    auto currentPosition1 = entity1->getEntityPosition();
    
    // 要リファクタリング
    // 予め動かしておいて
    entity1->setEntityPosition(currentPosition0);
    entity0->setEntityPosition(currentPosition1);
    
    EntityVector v0;
    EntityVector v1;
    auto entities0 = this->checkNeighborEntities(entity0, v0);
    auto entities1 = this->checkNeighborEntities(entity1, v1);
    
    bool isVanish = entities0.size() >= VANISH_COUNT || entities1.size() >= VANISH_COUNT;
    
    entity0->setState(Entity::State::SWAPPING);
    entity1->setState(Entity::State::SWAPPING);
    
    // 酷い実装だからあとで直す！！！！！
    
    if (!isVanish) { // 消えなかったら戻す
        entity1->setEntityPosition(currentPosition1);
        entity0->setEntityPosition(currentPosition0);
    }
    
    entity0->runAction(Sequence::create(MoveTo::create(duration, position1),
                                        CallFuncN::create([=](Node *node) {
        auto entity = dynamic_cast<Entity *>(node);
        if (isVanish) {
            this->moveEntity(entity, currentPosition1);
            entity->setState(Entity::State::NORMAL);
            this->checkField();
        } else {
            // 元に戻す
            entity->runAction(Sequence::create(MoveTo::create(duration / 2.0, position0),
                                               CallFuncN::create([] (Node *node) {
                auto entity = dynamic_cast<Entity *>(node);
                entity->setState(Entity::State::NORMAL);
            }), NULL));
        }
    }), NULL));
    entity1->runAction(Sequence::create(MoveTo::create(duration, position0),
                                        CallFuncN::create([=](Node *node) {
        auto entity = dynamic_cast<Entity *>(node);
        if (isVanish) {
            this->moveEntity(entity, currentPosition0);
            entity->setState(Entity::State::NORMAL);
            this->checkField();
        } else {
            // 元に戻す
            entity->runAction(Sequence::create(MoveTo::create(duration / 2.0, position1),
                                               CallFuncN::create([] (Node *node) {
                auto entity = dynamic_cast<Entity *>(node);
                entity->setState(Entity::State::NORMAL);
            }), NULL));
        }
    }),
                                        NULL));
    return true;
}

bool MainScene::checkVanishEntities(Entity *entity)
{
    EntityVector v;
    auto entities = this->checkNeighborEntities(entity, v);
    bool canVanish = std::all_of(entities.begin(), entities.end(), [](Entity *entity) { return entity->isNormal(); });
    if (entities.size() >= VANISH_COUNT && canVanish) {
        for (auto entity : entities) {
            this->deleteEntity(entity);
        }
        return true;
    }
    return false;
}

void MainScene::deleteEntity(Entity *entity)
{
    if (!entity) return;
    entity->setState(Entity::State::DISAPEARING);
    entity->runAction(Sequence::create(FadeOut::create(0.5f),
                                       CallFuncN::create([this](Node* node) {
        auto entity = dynamic_cast<Entity *>(node);
        _entities.eraseObject(entity);
    }),
                                       RemoveSelf::create(),
                                       CallFunc::create([this] {
    }),
                                       NULL));
}

EntityVector MainScene::checkNeighborEntities(Entity *entity, EntityVector checked) {
    if (checked.contains(entity)) {
        return checked;
    }
    checked.pushBack(entity);
    auto position = entity->getEntityPosition();
    auto up = this->getEntityAt(position.x, position.y + 1);
    auto down = this->getEntityAt(position.x, position.y - 1);
    auto left = this->getEntityAt(position.x - 1, position.y);
    auto right = this->getEntityAt(position.x + 1, position.y);
    
    if (up && up->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntities(up, checked);
    }
    if (down && down->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntities(down, checked);
    }
    if (left && left->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntities(left, checked);
    }
    if (right && right->getEntityColor() == entity->getEntityColor()) {
        checked = this->checkNeighborEntities(right, checked);
    }
    return std::move(checked);
}

void MainScene::checkFall(Entity *entity)
{
    auto position = entity->getEntityPosition();
    if (position.y == 0) {
        return;
    }
    if (!entity->isNormal()) {
        return;
    }
    auto downPosition = Vec2(position.x, position.y - 1);
    auto down = this->getEntityAt(position.x, position.y - 1);
    if (down == nullptr) {
        auto duration = 0.05;
        entity->setState(Entity::State::FALLING);
        entity->runAction(Sequence::create(MoveBy::create(duration, Vec2(0, -Entity::getSize())),
                                           CallFuncN::create([this, downPosition] (Node *node) {
            auto entity = dynamic_cast<Entity *>(node);
            this->moveEntity(entity, downPosition);
            entity->setState(Entity::State::NORMAL);
            this->checkField();
            this->checkFall(entity);
        }),
                                           NULL));
    }
}

cocos2d::Vector<Entity *> MainScene::spawnEntities()
{
    cocos2d::Vector<Entity *> entities;
    auto y = VERTICAL_COUNT - 1;
    for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
        auto entity = this->getEntityAt(x, y);
        if (!entity) {
            auto entity = Entity::create();
            entity->setEntityPosition(Vec2(x, y));
            this->addEntity(entity);
            entities.pushBack(entity);
            this->checkVanishEntities(entity);
        }
    }
    return std::move(entities);
}

bool MainScene::canVanishNext(Entity *entity)
{
    EntityVector entities;
    entities = this->checkNeighborEntities(entity, entities);
    auto vectors = std::vector<Vec2> {Vec2(0, 2), Vec2(2, 0), Vec2(0, -2), Vec2(-2, 0)};
    auto skews = std::vector<Vec2> {Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto allDirections = std::vector<Vec2> {Vec2(0, 2), Vec2(2, 0), Vec2(0, -2), Vec2(-2, 0), Vec2(1, 1), Vec2(1, -1), Vec2(-1, 1), Vec2(-1, -1)};
    auto currentVector = entity->getEntityPosition();
    
    if (entities.size() >= VANISH_COUNT) {
        // 4以上は存在しないはずだけどtrue
        return true;
    } else if (entities.size() == 3) {
        for (auto vector : allDirections) {
            auto nextVector = entity->getEntityPosition() + vector;
            auto nextEntity = this->getEntityAt(nextVector.x, nextVector.y);
            if (nextEntity && entity && nextEntity->getEntityColor() == entity->getEntityColor() && !entities.contains(nextEntity)) {
                return true;
            }
        }
    } else if (entities.size() == 2) {
        for (auto vector : vectors) {
            auto nextVector = entity->getEntityPosition() + vector;
            auto nextEntity = this->getEntityAt(nextVector.x, nextVector.y);
            if (nextEntity && entity && nextEntity->getEntityColor() == entity->getEntityColor() && !entities.contains(nextEntity)) {
                for (auto sv0 : skews) {
                    for (auto sv1 : skews) {
                        auto e0 = this->getEntityAt((currentVector + sv0).x, (currentVector + sv0).y);
                        auto e1 = this->getEntityAt((nextVector + sv1).x, (nextVector + sv1).y);
                        if (e0 && e1 && e0->getEntityColor() == entity->getEntityColor() && e0 == e1 && !entities.contains(e0) && !entities.contains(e1)) {
                            // 斜めに共通のentityがあれば消せるはず！
                            return true;
                        }
                    }
                }
            }
        }
    }
    return false;
}

void MainScene::checkField()
{
    if (this->isAllNormal()) {
        // 既に揃ってるのを消す
        EntityVector checked;
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto entity = this->getEntityAt(x, y);
                if (entity && !checked.contains(entity)) {
                    EntityVector v;
                    v = this->checkNeighborEntities(entity, v);
                    this->checkVanishEntities(entity);
                    checked.pushBack(v);
                }
            }
        }
        
        // 次にどれも消えなさそうだったらランダムに2列消す
        bool flag = false;
        for (int x = 0; x < HORIZONTAL_COUNT; ++x) {
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                auto entity = this->getEntityAt(x, y);
                if (!flag && entity && this->canVanishNext(entity)) {
                    // どれか消えそうなら探索を打ち切る
                    flag = true;
                }
            }
        }
        if (!flag && _entities.size() == HORIZONTAL_COUNT * VERTICAL_COUNT) {
            auto baseX = rand() % HORIZONTAL_COUNT;
            auto otherX = (rand() % (HORIZONTAL_COUNT - 1) + baseX) % HORIZONTAL_COUNT;
            for (int y = 0; y < VERTICAL_COUNT; ++y) {
                this->deleteEntity(this->getEntityAt(baseX, y));
                this->deleteEntity(this->getEntityAt(otherX, y));
            }
        }
    }
}

bool MainScene::isAllNormal()
{
    return std::all_of(_entities.begin(), _entities.end(), [](Entity* entity) { return entity->isNormal(); });
}