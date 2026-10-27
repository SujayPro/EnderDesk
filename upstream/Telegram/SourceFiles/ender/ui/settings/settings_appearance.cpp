// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ender/ui/settings/settings_appearance.h"

#include "lang_auto.h"
#include "ender/ender_settings.h"
#include "ender/ender_ui_settings.h"
#include "ender/ui/boxes/font_selector.h"
#include "ender/ui/components/avatar_corners_preview.h"
#include "ender/ui/components/icon_picker.h"
#include "ender/ui/settings/ender_builder.h"
#include "ender/ui/settings/settings_ender_utils.h"
#include "ender/ui/settings/settings_main.h"
#include "inline_bots/bot_attach_web_view.h"
#include "main/main_session.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_ender_icons.h"
#include "styles/style_ender_styles.h"
#include "styles/style_dialogs.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/painter.h"
#include "ui/widgets/labels.h"
#include "ui/wrap/padding_wrap.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace EnderBuilder;

namespace {

bool HasDrawerBots(not_null<Window::SessionController*> controller) {
	// todo: maybe iterate through all accounts
	const auto bots = &controller->session().attachWebView();
	for (const auto &bot : bots->attachBots()) {
		if (!bot.inMainMenu || !bot.media) {
			continue;
		}
		return true;
	}
	return false;
}

void BuildAppIcon(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle({
		.id = u"ender/appIcon"_q,
		.title = tr::ender_AppIconHeader(),
	});

	builder.add([](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		return {
			.widget = object_ptr<IconPicker>(ctx.container),
			.margin = st::settingsButtonNoIcon.padding,
		};
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	builder.addDivider();
	builder.addSkip();
	ender.addSettingToggle({
		.id = u"ender/hideNotificationBadge"_q,
		.title = tr::ender_HideNotificationBadge(),
		.getter = &EnderSettings::hideNotificationBadge,
		.setter = &EnderSettings::setHideNotificationBadge,
	});
	builder.addSkip();
	builder.addDividerText(tr::ender_HideNotificationBadgeDescription());
	builder.addSkip();
#else
    builder.addDivider();
    builder.addSkip();
#endif
}

void BuildAvatarCorners(SectionBuilder &builder, EnderSectionBuilder &ender) {
	auto *settings = &EnderSettings::getInstance();
	const auto controller = builder.controller();

	const auto mapRadius = [](int val)
	{
		if (val == 0) {
			return tr::ender_AvatarCornersSquare(tr::now).toUpper();
		} else if (val == EnderUiSettings::kMaxAvatarCorners) {
			return tr::ender_AvatarCornersCircle(tr::now).toUpper();
		}
		return QString::number(val);
	};

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		const auto container = ctx.container;
		auto title = object_ptr<Ui::FlatLabel>(
			container,
			tr::ender_AvatarCorners(),
			st::defaultSubsectionTitle);
		const auto titleRaw = title.data();

		const auto badge = Ui::CreateChild<Ui::PaddingWrap<Ui::FlatLabel>>(
			container,
			object_ptr<Ui::FlatLabel>(
				container,
				settings->avatarCornersValue() | rpl::map(mapRadius),
				st::settingsPremiumNewBadge),
			st::enderBetaBadgePadding);
		badge->show();
		badge->setAttribute(Qt::WA_TransparentForMouseEvents);
		badge->paintRequest() | rpl::on_next([=] {
			auto p = QPainter(badge);
			auto hq = PainterHighQualityEnabler(p);
			p.setPen(Qt::NoPen);
			p.setBrush(st::windowBgActive);
			const auto r = st::enderBetaBadgePadding.left();
			p.drawRoundedRect(badge->rect(), r, r);
		}, badge->lifetime());

		titleRaw->geometryValue() | rpl::on_next([=](QRect geometry) {
			badge->moveToLeft(
				geometry.x()
					+ titleRaw->textMaxWidth()
					+ st::settingsPremiumNewBadgePosition.x(),
				geometry.y()
					+ (geometry.height() - badge->height()) / 2);
		}, badge->lifetime());

		return {
			.widget = std::move(title),
			.margin = st::defaultSubsectionTitlePadding,
		};
	}, [] {
		return SearchEntry{
			.id = u"ender/avatarCorners"_q,
			.title = tr::ender_AvatarCorners(tr::now),
		};
	});

	auto *previewRaw = static_cast<AvatarCornersPreview*>(nullptr);
	builder.add([&](const Builder::WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<AvatarCornersPreview>(
			ctx.container,
			controller);
		previewRaw = preview.data();
		const auto vMargin = st::settingsButtonNoIcon.padding
			- st::defaultDialogRow.padding;
		return {
			.widget = std::move(preview),
			.margin = QMargins(0, vMargin.top(), 0, vMargin.bottom()),
		};
	});

	ender.addSlider({
		.id = u"ender/avatarCornersSlider"_q,
		.title = rpl::single(QString()),
		.showTitle = false,
		.steps = EnderUiSettings::kMaxAvatarCorners + 1,
		.current = settings->avatarCorners(),
		.onChanged = [=](int val) {
			EnderSettings::getInstance().setAvatarCorners(val);
			if (previewRaw) {
				previewRaw->update();
			}
		},
		.onFinalChanged = [=](int val) {
			EnderSettings::getInstance().setAvatarCorners(val);
			ShowRestartPrompt(controller);
		},
	});

	ender.addSettingToggle({
		.id = u"ender/singleCornerRadius"_q,
		.title = tr::ender_SingleCornerRadius(),
		.getter = &EnderSettings::singleCornerRadius,
		.setter = &EnderSettings::setSingleCornerRadius,
	});

	builder.addSkip();
	builder.addDividerText(tr::ender_SingleCornerRadiusDescription());
	builder.addSkip();
}

void BuildAppearance(SectionBuilder &builder, EnderSectionBuilder &ender) {
	auto *settings = &EnderSettings::getInstance();

	builder.addSubsectionTitle(tr::ender_CategoryAppearance());

	ender.addSettingToggle({
		.id = u"ender/materialSwitches"_q,
		.altIds = { u"ender/newSwitchStyle"_q },
		.title = tr::ender_MaterialSwitches(),
		.getter = &EnderSettings::materialSwitches,
		.setter = &EnderSettings::setMaterialSwitches,
	});
	ender.addSettingToggle({
		.id = u"ender/disableCustomBackgrounds"_q,
		.altIds = { u"ender/customThemes"_q },
		.title = tr::ender_DisableCustomBackgrounds(),
		.getter = &EnderSettings::disableCustomBackgrounds,
		.setter = &EnderSettings::setDisableCustomBackgrounds,
	});
	ender.addSettingToggle({
		.id = u"ender/hidePremiumStatuses"_q,
		.title = tr::ender_HidePremiumStatuses(),
		.getter = &EnderSettings::hidePremiumStatuses,
		.setter = &EnderSettings::setHidePremiumStatuses,
	});

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"ender/monoFont"_q,
		.title = tr::ender_MonospaceFont(),
		.st = &st::settingsButtonNoIcon,
		.label = rpl::single(
			settings->monoFont().isEmpty()
				? tr::ender_FontDefault(tr::now)
				: settings->monoFont()),
		.onClick = [=] {
			EnderUi::FontSelectorBox::Show(
				controller,
				[=](const QString &font) {
					EnderSettings::getInstance().setMonoFont(font);
				});
		},
	});

	ender.addSectionDivider();
}

void BuildChatFolders(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::ender_ChatFoldersHeader());

	ender.addSettingToggle({
		.id = u"ender/hideNotificationCounters"_q,
		.altIds = { u"ender/tabCounter"_q },
		.title = tr::ender_HideNotificationCounters(),
		.getter = &EnderSettings::hideNotificationCounters,
		.setter = &EnderSettings::setHideNotificationCounters,
	});
	ender.addSettingToggle({
		.id = u"ender/hideAllChatsFolder"_q,
		.altIds = { u"ender/hideAllChats"_q },
		.title = tr::ender_HideAllChats(),
		.getter = &EnderSettings::hideAllChatsFolder,
		.setter = &EnderSettings::setHideAllChatsFolder,
	});

	ender.addSectionDivider();
}

void BuildTrayElements(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::ender_TrayElementsHeader());

	ender.addSettingToggle({
		.id = u"ender/showGhostToggleInTray"_q,
		.title = tr::ender_EnableGhostModeTray(),
		.getter = &EnderSettings::showGhostToggleInTray,
		.setter = &EnderSettings::setShowGhostToggleInTray,
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	ender.addSettingToggle({
		.id = u"ender/showStreamerToggleInTray"_q,
		.title = tr::ender_EnableStreamerModeTray(),
		.getter = &EnderSettings::showStreamerToggleInTray,
		.setter = &EnderSettings::setShowStreamerToggleInTray,
	});
#endif

	ender.addSectionDivider();
}

void BuildDrawerElements(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::ender_DrawerElementsHeader());

	ender.addSettingToggle({
		.id = u"ender/showMyProfileInDrawer"_q,
		.title = tr::lng_menu_my_profile(),
		.getter = &EnderSettings::showMyProfileInDrawer,
		.setter = &EnderSettings::setShowMyProfileInDrawer,
		.icon = { &st::menuIconProfile },
	});

	const auto controller = builder.controller();
	if (controller && HasDrawerBots(controller)) {
		ender.addSettingToggle({
			.id = u"ender/showBotsInDrawer"_q,
			.title = tr::lng_filters_type_bots(),
			.getter = &EnderSettings::showBotsInDrawer,
			.setter = &EnderSettings::setShowBotsInDrawer,
			.icon = { &st::menuIconBot },
		});
	}

	ender.addSettingToggle({
		.id = u"ender/showNewGroupInDrawer"_q,
		.title = tr::lng_create_group_title(),
		.getter = &EnderSettings::showNewGroupInDrawer,
		.setter = &EnderSettings::setShowNewGroupInDrawer,
		.icon = { &st::menuIconGroups },
	});
	ender.addSettingToggle({
		.id = u"ender/showNewChannelInDrawer"_q,
		.title = tr::lng_create_channel_title(),
		.getter = &EnderSettings::showNewChannelInDrawer,
		.setter = &EnderSettings::setShowNewChannelInDrawer,
		.icon = { &st::menuIconChannel },
	});
	ender.addSettingToggle({
		.id = u"ender/showContactsInDrawer"_q,
		.title = tr::lng_menu_contacts(),
		.getter = &EnderSettings::showContactsInDrawer,
		.setter = &EnderSettings::setShowContactsInDrawer,
		.icon = { &st::menuIconUserShow },
	});
	ender.addSettingToggle({
		.id = u"ender/showCallsInDrawer"_q,
		.title = tr::lng_menu_calls(),
		.getter = &EnderSettings::showCallsInDrawer,
		.setter = &EnderSettings::setShowCallsInDrawer,
		.icon = { &st::menuIconPhone },
	});
	ender.addSettingToggle({
		.id = u"ender/showSavedMessagesInDrawer"_q,
		.title = tr::lng_saved_messages(),
		.getter = &EnderSettings::showSavedMessagesInDrawer,
		.setter = &EnderSettings::setShowSavedMessagesInDrawer,
		.icon = { &st::menuIconSavedMessages },
	});
	ender.addSettingToggle({
		.id = u"ender/showLReadToggleInDrawer"_q,
		.title = tr::ender_LReadMessages(),
		.getter = &EnderSettings::showLReadToggleInDrawer,
		.setter = &EnderSettings::setShowLReadToggleInDrawer,
		.icon = { &st::enderLReadMenuIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showSReadToggleInDrawer"_q,
		.title = tr::ender_SReadMessages(),
		.getter = &EnderSettings::showSReadToggleInDrawer,
		.setter = &EnderSettings::setShowSReadToggleInDrawer,
		.icon = { &st::enderSReadMenuIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showNightModeToggleInDrawer"_q,
		.title = tr::lng_menu_night_mode(),
		.getter = &EnderSettings::showNightModeToggleInDrawer,
		.setter = &EnderSettings::setShowNightModeToggleInDrawer,
		.icon = { &st::menuIconNightMode },
	});
	ender.addSettingToggle({
		.id = u"ender/showGhostToggleInDrawer"_q,
		.title = tr::ender_GhostModeToggle(),
		.getter = &EnderSettings::showGhostToggleInDrawer,
		.setter = &EnderSettings::setShowGhostToggleInDrawer,
		.icon = { &st::enderGhostIcon },
	});

#if defined Q_OS_WIN || defined Q_OS_MAC
	ender.addSettingToggle({
		.id = u"ender/showStreamerToggleInDrawer"_q,
		.title = tr::ender_StreamerModeToggle(),
		.getter = &EnderSettings::showStreamerToggleInDrawer,
		.setter = &EnderSettings::setShowStreamerToggleInDrawer,
		.icon = { &st::enderStreamerModeMenuIcon },
	});
#endif

	builder.addSkip();
}

const auto kMeta = BuildHelper({
	.id = EnderAppearance::Id(),
	.parentId = EnderMain::Id(),
	.title = &tr::ender_CategoryAppearance,
	.icon = &st::menuIconPalette,
}, [](SectionBuilder &builder) {
	auto ender = EnderSectionBuilder(builder);

	builder.addSkip();
	BuildAppIcon(builder, ender);
	BuildAvatarCorners(builder, ender);
	BuildAppearance(builder, ender);
	BuildChatFolders(builder, ender);
	BuildTrayElements(builder, ender);
	BuildDrawerElements(builder, ender);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> EnderAppearance::title() {
	return tr::ender_CategoryAppearance();
}

EnderAppearance::EnderAppearance(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void EnderAppearance::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type EnderAppearanceId() {
	return EnderAppearance::Id();
}

} // namespace Settings
