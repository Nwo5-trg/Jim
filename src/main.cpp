#include <Geode/Geode.hpp>
#include <Geode/modify/PlayLayer.hpp>

using namespace geode::prelude;

class $modify(PlayLayerHook, PlayLayer) {
    struct Fields {
        bool enabled = Mod::get()->getSettingValue<bool>("enabled");
        int distance = Mod::get()->getSettingValue<int64_t>("distance"); // percent
        float size = Mod::get()->getSettingValue<double>("size");
        float entryDuration = Mod::get()->getSettingValue<double>("entry-duration");
        float exitDelay = Mod::get()->getSettingValue<double>("exit-delay");
        float exitDuration = Mod::get()->getSettingValue<double>("exit-duration");
        CCPoint offset = ccp(
            Mod::get()->getSettingValue<double>("offset-x"),
            Mod::get()->getSettingValue<double>("offset-y")
        );
        std::filesystem::path path = Mod::get()->getSettingValue<std::filesystem::path>("path");

        bool showedJim = false;
        bool hiddenJim = false;
        CCSprite* jim = nullptr;
    };

    bool init(GJGameLevel* level, bool useReplay, bool dontCreateObjects) {
        if (!PlayLayer::init(level, useReplay, dontCreateObjects)) return false;

        if (!m_fields->enabled) return true;
        
        // yoinked from doki mod cuz its prolly safe
        auto str = string::pathToString(m_fields->path);
        m_fields->jim = CCSprite::create(str.c_str());
        if (!m_fields->jim || str.empty() || !std::filesystem::exists(m_fields->path)) {
            m_fields->jim = CCSprite::create("jim.png"_spr);
        }
        else {
            CCImage* image = new CCImage();
            image->initWithImageFile(str.c_str());

            CCTexture2D* texture = new CCTexture2D();
            texture->initWithImage(image);

            m_fields->jim = CCSprite::createWithTexture(texture);
        }

        m_fields->jim->setID("jim"_spr);
        this->addChild(m_fields->jim);

        m_fields->jim->setScale(
            m_fields->size / 
            std::max(m_fields->jim->getContentWidth(), m_fields->jim->getContentHeight()) 
        );

        resetJim();

        return true;
    }

    void resetLevel() {
        resetJim();

        PlayLayer::resetLevel();
    }

    void updateProgressbar() {
        PlayLayer::updateProgressbar();

        if (m_level->m_normalPercent == 0 || m_isPlatformer || !m_startingFromBeginning || !m_fields->enabled) return;

        int percent = getCurrentPercentInt();
        if (!m_inResetDelay && !m_fields->showedJim && percent > m_level->m_normalPercent - m_fields->distance) {
            showJim();
        }
        else if (!m_fields->hiddenJim && percent > m_level->m_normalPercent + m_fields->exitDelay) {
            hideJim();
        }
    }

    void resetJim() {
        if (!m_fields->enabled) return;

        m_fields->showedJim = false;
        m_fields->hiddenJim = false;

        m_fields->jim->setVisible(false);

        auto winSize = CCDirector::get()->getWinSize();
        m_fields->jim->setPosition({winSize.width + (m_fields->size / 2), winSize.height / 2});

        m_fields->jim->stopAllActions();
    }

    void showJim() {
        m_fields->showedJim = true;

        m_fields->jim->setVisible(true);

        auto winSize = CCDirector::get()->getWinSize();
        m_fields->jim->runAction(CCMoveTo::create(
            m_fields->entryDuration,
            ccp(winSize.width - (m_fields->size / 2), winSize.height / 2) + m_fields->offset
        ));
    }

    void hideJim() {
        m_fields->hiddenJim = true;

        auto winSize = CCDirector::get()->getWinSize();
        m_fields->jim->runAction(CCMoveTo::create(
            m_fields->exitDuration,
            {m_fields->jim->getPositionX(), winSize.height + (m_fields->size / 2)}
        ));
    }
};