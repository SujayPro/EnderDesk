// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ender/ui/settings/settings_filters.h"

#include "lang_auto.h"
#include "ender/ender_settings.h"
#include "ender/data/ender_database.h"
#include "ender/features/filters/filters_cache_controller.h"
#include "ender/ui/boxes/import_filters_box.h"
#include "ender/ui/settings/ender_builder.h"
#include "ender/ui/settings/settings_main.h"
#include "ender/utils/telegram_helpers.h"
#include "boxes/abstract_box.h"
#include "boxes/peer_list_box.h"
#include "core/application.h"
#include "filters/per_dialog_filter.h"
#include "filters/settings_filters_list.h"
#include "inline_bots/bot_attach_web_view.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_ender_icons.h"
#include "styles/style_boxes.h"
#include "styles/style_layers.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/vertical_list.h"
#include "ui/boxes/confirm_box.h"
#include "ui/widgets/buttons.h"
#include "ui/widgets/menu/menu_add_action_callback.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_controller.h"
#include "window/window_peer_menu.h"
#include "window/window_session_controller.h"

namespace Settings {

using namespace Builder;
using namespace EnderBuilder;

namespace {

void BuildFiltersSettings(SectionBuilder &builder) {
	auto *settings = &EnderSettings::getInstance();

	builder.addSkip();
	builder.addSubsectionTitle(tr::ender_RegexFilters());

	const auto enabledButton = builder.addButton({
		.id = u"ender/filtersEnabled"_q,
		.title = tr::ender_RegexFiltersEnable(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->filtersEnabled()),
	});
	if (enabledButton) {
		enabledButton->toggledValue(
		) | rpl::filter([=](bool enabled) {
			return (enabled != settings->filtersEnabled());
		}) | on_next([=](bool enabled) {
			EnderSettings::getInstance().setFiltersEnabled(enabled);
			FiltersCacheController::rebuildCache();
			FiltersCacheController::fireUpdate();
		}, enabledButton->lifetime());
	}

	const auto sharedButton = builder.addButton({
		.id = u"ender/filtersEnabledInChats"_q,
		.altIds = { u"ender/filtersInChats"_q },
		.title = tr::ender_RegexFiltersEnableSharedInChats(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->filtersEnabledInChats()),
	});
	if (sharedButton) {
		sharedButton->toggledValue(
		) | rpl::filter([=](bool enabled) {
			return (enabled != settings->filtersEnabledInChats());
		}) | on_next([=](bool enabled) {
			EnderSettings::getInstance().setFiltersEnabledInChats(enabled);
			FiltersCacheController::rebuildCache();
			FiltersCacheController::fireUpdate();
		}, sharedButton->lifetime());
	}

	const auto blockedButton = builder.addButton({
		.id = u"ender/hideFromBlocked"_q,
		.title = tr::ender_FiltersHideFromBlocked(),
		.st = &st::settingsButtonNoIcon,
		.toggled = rpl::single(settings->hideFromBlocked()),
	});
	if (blockedButton) {
		blockedButton->toggledValue(
		) | rpl::filter([=](bool enabled) {
			return (enabled != settings->hideFromBlocked());
		}) | on_next([=](bool enabled) {
			EnderSettings::getInstance().setHideFromBlocked(enabled);
			FiltersCacheController::rebuildCache();
			FiltersCacheController::fireUpdate();
		}, blockedButton->lifetime());
	}

	builder.addSkip();
}

void BuildShared(SectionBuilder &builder) {
	builder.addDivider();
	builder.addSkip();

	const auto controller = builder.controller();
	builder.addButton({
		.id = u"ender/sharedFilters"_q,
		.title = tr::ender_RegexFiltersShared(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] {
			controller->dialogId = std::nullopt;
			controller->showExclude = false;
			controller->showSettings(EnderFiltersList::Id());
		},
	});
}

void BuildShadowBan(SectionBuilder &builder) {
	const auto controller = builder.controller();

	builder.addButton({
		.id = u"ender/shadowBanIds"_q,
		.altIds = { u"ender/shadowBanList"_q },
		.title = tr::ender_FiltersShadowBan(),
		.st = &st::settingsButtonNoIcon,
		.onClick = [=] {
			controller->dialogId = std::nullopt;
			controller->showExclude = false;
			controller->shadowBan = true;
			controller->showSettings(EnderFiltersList::Id());
		},
	});
}

void BuildPerDialog(SectionBuilder &builder) {
	builder.add([](const BuildContext &ctx) {
		v::match(ctx, [&](const WidgetContext &wctx) {
			if (!EnderDatabase::hasPerDialogFilters()) {
				return;
			}

			const auto container = wctx.container;
			const auto controller = wctx.controller;

			AddSkip(container);
			AddDivider(container);

			auto ctrl = container->lifetime().make_state<PerDialogFiltersListController>(
				&controller->session(),
				controller);

			auto list = object_ptr<Ui::PaddingWrap<PeerListContent>>(
				container,
				object_ptr<PeerListContent>(
					container,
					ctrl),
				QMargins(0, -st::peerListBox.padding.top(), 0, -st::peerListBox.padding.bottom()));
			AddSkip(container);
			const auto content = container->add(std::move(list));
			AddSkip(container);
			auto delegate = container->lifetime().make_state<PeerListContentDelegateSimple>();
			delegate->setContent(content->entity());
			ctrl->setDelegate(delegate);
		}, [&](const SearchContext &) {
		});
	});
}

const auto kMeta = BuildHelper({
	.id = EnderFilters::Id(),
	.parentId = EnderMain::Id(),
	.title = &tr::ender_CategoryFilters,
	.icon = &st::menuIconTagFilter,
}, [](SectionBuilder &builder) {
	BuildFiltersSettings(builder);
	BuildShared(builder);
	BuildShadowBan(builder);
	BuildPerDialog(builder);
});

} // namespace

rpl::producer<QString> EnderFilters::title() {
	return tr::ender_CategoryFilters();
}

void EnderFilters::fillTopBarMenu(const Ui::Menu::MenuCallback &addAction) {
	addAction(
		tr::ender_FiltersMenuSelectChat(tr::now),
		[=] {
			if (const auto window = Core::App().activeWindow()) {
				if (const auto controller = window->sessionController()) {
					auto types = InlineBots::PeerTypes();
					types |= InlineBots::PeerType::Bot;
					types |= InlineBots::PeerType::Group;
					types |= InlineBots::PeerType::Broadcast;

					Window::ShowChooseRecipientBox(
						controller,
						[=](not_null<Data::Thread*> thread) {
							const auto peer = thread->peer();
							controller->dialogId = getDialogIdFromPeer(peer);
							controller->showExclude = true;
							controller->showSettings(EnderFiltersList::Id());
							return true;
						},
						tr::ender_FiltersMenuSelectChat(),
						nullptr,
						types);
				}
			}
		},
		&st::menuIconSearch);
	addAction({ .isSeparator = true });
	addAction(
		tr::ender_FiltersMenuImport(tr::now),
		[=] {
			auto box = Box(Ui::FillImportFiltersBox, true);
			Ui::show(std::move(box));
		},
		&st::menuIconArchive);
	if (EnderDatabase::hasFilters()) {
		addAction(
			tr::ender_FiltersMenuExport(tr::now),
			[=] {
				auto box = Box(Ui::FillImportFiltersBox, false);
				Ui::show(std::move(box));
			},
			&st::menuIconUnarchive);
	}
	addAction({ .isSeparator = true });
	addAction({
		.text = tr::ender_FiltersMenuClear(tr::now),
		.handler = [=] {
			auto callback = [=](Fn<void()> &&close) {
				EnderDatabase::deleteAllFilters();
				EnderDatabase::deleteAllExclusions();
				FiltersCacheController::rebuildCache();
				FiltersCacheController::fireUpdate();
				close();
			};
			auto box = Ui::MakeConfirmBox({
				.text = tr::ender_FiltersClearPopupText(),
				.confirmed = callback,
				.confirmText = tr::ender_FiltersClearPopupActionText(),
				.confirmStyle = &st::attentionBoxButton,
			});
			Ui::show(std::move(box));
		},
		.icon = &st::menuIconClearAttention,
		.isAttention = true,
	});
}

EnderFilters::EnderFilters(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void EnderFilters::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type EnderFiltersId() {
	return EnderFilters::Id();
}

} // namespace Settings
