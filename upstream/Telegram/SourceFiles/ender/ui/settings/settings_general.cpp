// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ender/ui/settings/settings_general.h"

#include "lang_auto.h"
#include "ender/ender_settings.h"
#include "ender/ui/settings/ender_builder.h"
#include "ender/ui/settings/settings_ender_utils.h"
#include "ender/ui/settings/settings_main.h"
#include "base/platform/base_platform_info.h"
#include "core/application.h"
#include "lang/lang_text_entity.h"
#include "platform/platform_translate_provider.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/boxes/single_choice_box.h"
#include "ui/toast/toast.h"
#include "ui/widgets/buttons.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace EnderBuilder;

namespace {

void BuildTranslator(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::lng_translate_settings_subtitle());

	auto *settings = &EnderSettings::getInstance();

	const auto options = std::vector{
		std::pair(TranslationProvider::Telegram, QString("Telegram")),
		std::pair(TranslationProvider::Google, QString("Google")),
		std::pair(TranslationProvider::Yandex, QString("Yandex")),
	};
	const auto nativeAvailable = Platform::IsTranslateProviderAvailable();
	auto availableOptions = options;
	if (nativeAvailable) {
		availableOptions.push_back(std::pair(
			TranslationProvider::Native,
			[] {
				if constexpr (Platform::IsMac()) {
					return QString("macOS");
				} else if constexpr (Platform::IsWindows()) {
					return QString("Windows");
				} else {
					return QString("Linux");
				}
			}()));
	}
	auto optionLabels = std::vector<QString>();
	optionLabels.reserve(availableOptions.size());
	for (const auto &option : availableOptions) {
		optionLabels.push_back(option.second);
	}

	const auto getIndex = [=](TranslationProvider val) {
		const auto i = ranges::find(
			availableOptions,
			val,
			&std::pair<TranslationProvider, QString>::first);
		return (i != end(availableOptions))
			? int(i - begin(availableOptions))
			: 0;
	};

	auto currentVal = EnderSettings::getInstance().translationProviderValue()
		| rpl::map(getIndex)
		| rpl::map([=](int val) { return availableOptions[val].second; });

	const auto button = builder.addButton({
		.id = u"ender/translationProvider"_q,
		.title = tr::ender_TranslationProvider(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			if (const auto controller = Core::App().activeWindow()->sessionController()) {
				controller->show(Box(
						[=](not_null<Ui::GenericBox*> box) {
							const auto save = [=](int index) {
								const auto option = availableOptions[index].first;
								EnderSettings::getInstance().setTranslationProvider(option);

								if constexpr (Platform::IsMac()) {
									if (option == TranslationProvider::Native) {
										controller->showToast(Ui::Toast::Config{
											.text = tr::lng_translate_settings_use_platform_mac_about(tr::now, tr::rich),
											.duration = 6 * crl::time(1000)
										});
									}
								}
							};
							SingleChoiceBox(box, {
								.title = tr::ender_TranslationProvider(),
								.options = optionLabels,
								.initialSelection = getIndex(settings->translationProvider()),
								.callback = save,
							});
						}));
			}
		},
	});
	if (button) {
		ender.addBetaBadge(button);
	}
}

void BuildShowPeerId(SectionBuilder &builder) {
	auto *settings = &EnderSettings::getInstance();

	const auto options = std::vector{
		QString(tr::ender_SettingsShowID_Hide(tr::now)),
		QString("Telegram API"),
		QString("Bot API")
	};

	auto currentVal = EnderSettings::getInstance().showPeerIdValue()
		| rpl::map([=](PeerIdDisplay val) {
			return options[static_cast<int>(val)];
		});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"ender/showPeerId"_q,
		.altIds = { u"ender/showIdAndDc"_q },
		.title = tr::ender_SettingsShowID(),
		.st = &st::settingsButtonNoIcon,
		.label = std::move(currentVal),
		.onClick = [=] {
			controller->show(Box(
				[=](not_null<Ui::GenericBox*> box) {
					const auto save = [=](int index) {
						EnderSettings::getInstance().setShowPeerId(
							static_cast<PeerIdDisplay>(index));
					};
					SingleChoiceBox(box, {
						.title = tr::ender_SettingsShowID(),
						.options = options,
						.initialSelection = static_cast<int>(settings->showPeerId()),
						.callback = save,
					});
				}));
		},
	});
}

void BuildQoLToggles(SectionBuilder &builder, EnderSectionBuilder &ender) {
	auto *settings = &EnderSettings::getInstance();

	BuildTranslator(builder, ender);
	ender.addSectionDivider();

	builder.addSubsectionTitle(tr::ender_CategoryGeneral());

	const auto controller = builder.controller();
	ender.addToggle({
		.id = u"ender/disableStories"_q,
		.altIds = { u"ender/hideStories"_q },
		.title = tr::ender_DisableStories(),
		.getter = [=] { return settings->disableStories(); },
		.setter = [=](bool enabled) {
			EnderSettings::getInstance().setDisableStories(enabled);
			ShowRestartPrompt(controller);
		},
	});

	ender.addSettingToggle({
		.id = u"ender/disableOpenLinkWarning"_q,
		.title = tr::ender_DisableOpenLinkWarning(),
		.getter = &EnderSettings::disableOpenLinkWarning,
		.setter = &EnderSettings::setDisableOpenLinkWarning,
	});

	ender.addCollapsibleToggle({
		.id = u"ender/similarChannels"_q,
		.title = tr::ender_DisableSimilarChannels(),
		.checkboxes = {
			NestedEntry{
				tr::ender_CollapseSimilarChannels(tr::now),
				[] { return EnderSettings::getInstance().collapseSimilarChannels(); },
				[](bool v) { EnderSettings::getInstance().setCollapseSimilarChannels(v); }
			},
			NestedEntry{
				tr::ender_HideSimilarChannelsTab(tr::now),
				[] { return EnderSettings::getInstance().hideSimilarChannels(); },
				[](bool v) { EnderSettings::getInstance().setHideSimilarChannels(v); }
			}
		},
		.toggledWhenAll = true,
	});

	ender.addSettingToggle({
		.id = u"ender/disableNotificationsDelay"_q,
		.title = tr::ender_DisableNotificationsDelay(),
		.getter = &EnderSettings::disableNotificationsDelay,
		.setter = &EnderSettings::setDisableNotificationsDelay,
	});

	ender.addSectionDivider();

	const auto zalgoButton = builder.addButton({
		.id = u"ender/filterZalgo"_q,
		.title = tr::ender_FilterZalgo(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->filterZalgo()),
	});
	if (zalgoButton) {
		zalgoButton->toggledValue(
		) | rpl::filter(
			[=](bool enabled) {
				return (enabled != settings->filterZalgo());
			}
		) | on_next(
			[=](bool enabled) {
				EnderSettings::getInstance().setFilterZalgo(enabled);
				ShowRestartPrompt(controller);
			},
			zalgoButton->lifetime());
		ender.addBetaBadge(zalgoButton);
	}

	ender.addSettingToggle({
		.id = u"ender/improveLinkPreviews"_q,
		.title = tr::ender_ImproveLinkPreviews(),
		.getter = &EnderSettings::improveLinkPreviews,
		.setter = &EnderSettings::setImproveLinkPreviews,
	});
	ender.addCollapsibleToggle({
		.id = u"ender/confirmations"_q,
		.title = tr::ender_ConfirmationsTitle(),
		.checkboxes = {
			NestedEntry{
				tr::ender_StickerConfirmation(tr::now),
				[] { return EnderSettings::getInstance().stickerConfirmation(); },
				[](bool v) { EnderSettings::getInstance().setStickerConfirmation(v); }
			},
			NestedEntry{
				tr::ender_GIFConfirmation(tr::now),
				[] { return EnderSettings::getInstance().gifConfirmation(); },
				[](bool v) { EnderSettings::getInstance().setGifConfirmation(v); }
			},
			NestedEntry{
				tr::ender_VoiceConfirmation(tr::now),
				[] { return EnderSettings::getInstance().voiceConfirmation(); },
				[](bool v) { EnderSettings::getInstance().setVoiceConfirmation(v); }
			},
			NestedEntry{
				tr::ender_RoundConfirmation(tr::now),
				[] { return EnderSettings::getInstance().roundConfirmation(); },
				[](bool v) { EnderSettings::getInstance().setRoundConfirmation(v); }
			}
		},
		.toggledWhenAll = false,
	});
	ender.addSettingToggle({
		.id = u"ender/showMessageSeconds"_q,
		.altIds = { u"ender/formatTimeWithSeconds"_q },
		.title = tr::ender_SettingsShowMessageSeconds(),
		.getter = &EnderSettings::showMessageSeconds,
		.setter = &EnderSettings::setShowMessageSeconds,
	});

	BuildShowPeerId(builder);

	ender.addSectionDivider();

	builder.addSubsectionTitle(rpl::single(QString("Webview")));

	ender.addSettingToggle({
		.id = u"ender/spoofWebviewAsAndroid"_q,
		.title = tr::ender_SettingsSpoofWebviewAsAndroid(),
		.getter = &EnderSettings::spoofWebviewAsAndroid,
		.setter = &EnderSettings::setSpoofWebviewAsAndroid,
	});

	ender.addCollapsibleToggle({
		.id = u"ender/biggerWindow"_q,
		.title = tr::ender_SettingsBiggerWindow(),
		.checkboxes = {
			NestedEntry{
				tr::ender_SettingsIncreaseWebviewHeight(tr::now),
				[] { return EnderSettings::getInstance().increaseWebviewHeight(); },
				[](bool v) { EnderSettings::getInstance().setIncreaseWebviewHeight(v); }
			},
			NestedEntry{
				tr::ender_SettingsIncreaseWebviewWidth(tr::now),
				[] { return EnderSettings::getInstance().increaseWebviewWidth(); },
				[](bool v) { EnderSettings::getInstance().setIncreaseWebviewWidth(v); }
			}
		},
		.toggledWhenAll = false,
	});
}

const auto kMeta = BuildHelper({
	.id = EnderGeneral::Id(),
	.parentId = EnderMain::Id(),
	.title = &tr::ender_CategoryGeneral,
	.icon = &st::menuIconShowAll,
}, [](SectionBuilder &builder) {
	auto ender = EnderSectionBuilder(builder);

	builder.addSkip();
	BuildQoLToggles(builder, ender);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> EnderGeneral::title() {
	return tr::ender_CategoryGeneral();
}

EnderGeneral::EnderGeneral(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void EnderGeneral::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type EnderGeneralId() {
	return EnderGeneral::Id();
}

} // namespace Settings
