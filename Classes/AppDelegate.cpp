#include "AppDelegate.h"
#include "HelloWorldScene.h"
#include "audio/include/AudioEngine.h"
#include "audio/include/SimpleAudioEngine.h"

USING_NS_CC;

using namespace cocos2d::experimental;

AppDelegate::AppDelegate()
{
    CCFileUtils* fileUtils = CCFileUtils::sharedFileUtils();
    std::vector<std::string> searchPaths = fileUtils->getSearchPaths();
    constexpr auto INSTALL_DIR = "/usr/share/Peel-The-Potatoes/Resources";
    searchPaths.push_back(INSTALL_DIR);

    if (auto appDir = getenv("APPDIR"))
    {
        const std::string appDirPrefix = appDir != nullptr ? appDir : "";
        searchPaths.push_back(appDirPrefix + INSTALL_DIR);
    }

    fileUtils->setSearchPaths(searchPaths);
}

AppDelegate::~AppDelegate() {}

// if you want a different context,just modify the value of glContextAttrs
// it will takes effect on all platforms
void AppDelegate::initGLContextAttrs()
{
    // set OpenGL context attributions,now can only set six attributions:
    // red,green,blue,alpha,depth,stencil
    GLContextAttrs glContextAttrs = {8, 8, 8, 8, 24, 8};

    GLView::setGLContextAttrs(glContextAttrs);
}

// If you want to use packages manager to install more packages,
// don't modify or remove this function
static int register_all_packages()
{
    return 0; // flag for packages manager
}

bool AppDelegate::applicationDidFinishLaunching()
{
    // initialize director
    auto director = Director::getInstance();
    auto glview = director->getOpenGLView();
    if (!glview)
    {
        glview = GLViewImpl::createWithRect("Peel The Potatoes!",
                                            Rect(0, 0, 640, 400));
        director->setOpenGLView(glview);
    }

    // set FPS. the default value is 1.0/60 if you don't call this
    director->setAnimationInterval(1.0 / 60);

    register_all_packages();

    // create a scene. it's an autorelease object
    auto scene = HelloWorld::createScene();

    // run
    director->runWithScene(scene);

    return true;
}

// This function will be called when the app is inactive. When comes a phone
// call,it's be invoked too
void AppDelegate::applicationDidEnterBackground()
{
    Director::getInstance()->stopAnimation();

    AudioEngine::pauseAll();
}

// this function will be called when the app is active again
void AppDelegate::applicationWillEnterForeground()
{
    Director::getInstance()->startAnimation();

    // AudioEngine::resumeAll();
}
