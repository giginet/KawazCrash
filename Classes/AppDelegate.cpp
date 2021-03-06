#include "AppDelegate.h"
#include "LicenseScene.h"
#include "ADX2Manager.h"
#include "SharedCueSheet.h"

USING_NS_CC;

AppDelegate::AppDelegate() {
    
}

AppDelegate::~AppDelegate()
{
}

bool AppDelegate::applicationDidFinishLaunching() {
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if(!glview) {
        glview = GLView::create("My Game");
        director->setOpenGLView(glview);
    }
    
    // ADX2を初期化します
    CriAtomExStandardVoicePoolConfig vp_config;
    criAtomExVoicePool_SetDefaultConfigForStandardVoicePool(&vp_config);
    vp_config.num_voices = 8;
    vp_config.player_config.streaming_flag = CRI_TRUE;
    vp_config.player_config.max_sampling_rate = 48000 << 1;
    
    CriAtomExPlayerConfig pf_config;
    criAtomExPlayer_SetDefaultConfig(&pf_config);
    pf_config.max_path_strings = 1;
    pf_config.max_path = 256;
    
    ADX2::Manager::initialize(pf_config, vp_config);
    
    SharedCueSheet::getInstance()->initialize("adx2/cookie/cookie_crush.acf",
                                              "adx2/cookie/cookie_main.acb");
    
    // turn on display FPS
    director->setDisplayStats(false);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    glview->setDesignResolutionSize(320, 568, ResolutionPolicy::SHOW_ALL);
    
    // Resourceの探索パスを設定する
    auto addPath = [](const char* path) {
        FileUtils::getInstance()->addSearchPath(path);
    };
    addPath("music");
    addPath("se");
    
    // 画像リソースを振り分ける
    // 画像リソースを検索する順番を定義する配列
    std::vector<std::string> searchResolutionOrder;
    auto platform = this->getTargetPlatform(); // 現在のプラットフォーム
    auto frameSize = glview->getFrameSize(); // 現在の端末の画面サイズ
    
    if (platform == Platform::OS_IPHONE) { // もし、iPhoneなら
        if (frameSize.height > 480.f) {
            // Retinaディスプレイのとき
            // 高解像度画像を有効にする
            director->setContentScaleFactor(2.0f);
            if (frameSize.height == 1136) {
                // iPhone 4inchのとき
                // 4インチ対応の画面サイズに変更する
                glview->setDesignResolutionSize(320, 568, ResolutionPolicy::NO_BORDER);
                // Resources/4inchフォルダに画像ファイルがあれば、最優先で利用する
                searchResolutionOrder.push_back("images/4inch");
            } else {
                // Retina 3.5インチのとき
                glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
            }
            searchResolutionOrder.push_back("images/retina");
        } else { // non-Retina 3.5インチ
            glview->setDesignResolutionSize(320, 480, ResolutionPolicy::SHOW_ALL);
        }
    } else if (platform == Platform::OS_ANDROID) {
        // Android端末のとき
        director->setContentScaleFactor(2.0f);
        // iPhone 4inchのとき
        glview->setDesignResolutionSize(320, 568, ResolutionPolicy::SHOW_ALL);
        // Resources/4inchフォルダに画像ファイルがあれば、最優先で利用する
        searchResolutionOrder.push_back("images/retina");
    }
    searchResolutionOrder.push_back("images/nonretina");
    // 画像の読み込み順を設定する
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);
    
    // ADX2の設定
    auto fp = [](const char* filename)
    {
#if (CC_TARGET_PLATFORM == CC_PLATFORM_IOS)
        return cocos2d::FileUtils::getInstance()->fullPathForFilename(filename);
#elif (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID) || (CC_TARGET_PLATFORM == CC_PLATFORM_WIN32)
        /* assetsへのアクセスパスは、必ずassets以下からの相対パスでファイルを指定する
         * パスの先頭に'/'文字が入ってはダメ */
        return std::string(filename);
#endif
    };
    
    auto acfPath = fp("adx2/cookie/cookie_crush.acf");
    auto acbPath = fp("adx2/cookie/cookie_main.acb");
    
    criAtomEx_RegisterAcfFile(NULL, acfPath.c_str(), NULL, 0);
    
    // create a scene. it's an autorelease object
    auto scene = LicenseScene::createScene();
    
    // run
    director->runWithScene(scene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // サスペンド時は音声を一時停止
    criAtomEx_StopSound_ANDROID();
#endif
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
    
#if (CC_TARGET_PLATFORM == CC_PLATFORM_ANDROID)
    // リジューム時は音声を再開
    criAtomEx_StartSound_ANDROID();
#endif
}
