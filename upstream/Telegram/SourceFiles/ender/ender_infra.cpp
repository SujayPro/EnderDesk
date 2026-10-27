// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ender/ender_infra.h"

#include "ender/ender_lang.h"
#include "ender/ender_settings.h"
#include "ender/ender_ui_settings.h"
#include "ender/ender_worker.h"
#include "ender/data/ender_database.h"
#include "ender/ui/ender_logo.h"
#include "features/translator/ender_translator.h"
#include "lang/lang_instance.h"
#include "ui/chat/chat_style_radius.h"
#include "utils/rc_manager.h"

#ifdef Q_OS_WIN
#include "ender/utils/windows_utils.h"
#endif

namespace EnderInfra {

void initLang() {
	QString id = Lang::GetInstance().id();
	QString baseId = Lang::GetInstance().baseId();
	if (id.isEmpty()) {
		LOG(("Language is not loaded"));
		return;
	}
	EnderLanguage::init();
	EnderLanguage::currentInstance()->fetchLanguage(id, baseId);
}

void initUiSettings() {
	const auto &settings = EnderSettings::getInstance();

	EnderUiSettings::setMonoFont(settings.monoFont());
	EnderUiSettings::setWideMultiplier(settings.wideMultiplier());
	EnderUiSettings::setMaterialSwitches(settings.materialSwitches());
	EnderUiSettings::setAvatarCorners(settings.avatarCorners());
	Ui::SetAppliedBubbleRadius(settings.messageBubbleRadius());
}

void initDatabase() {
	EnderDatabase::initialize();
}

void initWorker() {
	EnderWorker::initialize();
}

void initRCManager() {
	RCManager::getInstance().start();
}

void initTranslator() {
	Ender::Translator::TranslateManager::init();
}

void initIcon() {
#ifdef Q_OS_WIN
	EnderAssets::loadAppIco();
	reloadAppIconFromTaskBar();
#endif
}

void init() {
	initLang();
	initDatabase();
	initUiSettings();
	initIcon();
	initWorker();
	initRCManager();
	initTranslator();
}

}
