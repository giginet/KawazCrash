#include "AppDelegate.h"
#include "MainScene.h"

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
    
    // turn on display FPS
    director->setDisplayStats(true);
    
    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);
    
    glview->setDesignResolutionSize(320, 568, ResolutionPolicy::EXACT_FIT);
    
    // Resourceの探索パスを設定する
    auto addPath = [](const char* path) {
        FileUtils::getInstance()->addSearchPath(path);
    };
    addPath("music");
    addPath("se");
    addPath("adx2/test");
    addPath("adx2/main");
    
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
            glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
        }
    } else if (platform == Platform::OS_ANDROID) {
        // Android端末のとき
        glview->setDesignResolutionSize(320, 480, ResolutionPolicy::NO_BORDER);
    }
    searchResolutionOrder.push_back("images/nonretina");
    // 画像の読み込み順を設定する
    FileUtils::getInstance()->setSearchResolutionsOrder(searchResolutionOrder);
    
    // create a scene. it's an autorelease object
    auto scene = MainScene::createScene();
    
    // run
    director->runWithScene(scene);
    
    return true;
}

// This function will be called when the app is inactive. When comes a phone call,it's be invoked too
void AppDelegate::applicationDidEnterBackground() {
    Director::getInstance()->stopAnimation();
    
    // if you use SimpleAudioEngine, it must be pause
    // SimpleAudioEngine::getInstance()->pauseBackgroundMusic();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground() {
    Director::getInstance()->startAnimation();
    
    // if you use SimpleAudioEngine, it must resume here
    // SimpleAudioEngine::getInstance()->resumeBackgroundMusic();
}
