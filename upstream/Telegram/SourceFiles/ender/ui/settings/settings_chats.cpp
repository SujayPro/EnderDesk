// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#include "ender/ui/settings/settings_chats.h"

#include "lang_auto.h"
#include "ender/ender_settings.h"
#include "ender/ui/boxes/edit_mark_box.h"
#include "ender/ui/components/message_preview.h"
#include "ender/ui/settings/ender_builder.h"
#include "ender/ui/settings/settings_ender_utils.h"
#include "ender/ui/settings/settings_main.h"
#include "settings/settings_builder.h"
#include "settings/settings_common.h"
#include "styles/style_ender_icons.h"
#include "styles/style_menu_icons.h"
#include "styles/style_settings.h"
#include "ui/wrap/vertical_layout.h"
#include "window/window_session_controller.h"

#include <memory>

namespace Settings {

using namespace Builder;
using namespace EnderBuilder;

namespace {

struct PreviewState {
	MessagePreview *widget = nullptr;
};

void BuildStickersAndEmoji(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::lng_settings_stickers_emoji());

	ender.addSettingToggle({
		.id = u"ender/showOnlyAddedEmojisAndStickers"_q,
		.title = tr::ender_ShowOnlyAddedEmojisAndStickers(),
		.getter = &EnderSettings::showOnlyAddedEmojisAndStickers,
		.setter = &EnderSettings::setShowOnlyAddedEmojisAndStickers,
	});

	ender.addSettingToggle({
		.id = u"ender/unlimitedRecentStickers"_q,
		.altIds = { u"ender/recentStickersCount"_q },
		.title = tr::ender_SettingsUnlimitedRecentStickers(),
		.getter = &EnderSettings::unlimitedRecentStickers,
		.setter = &EnderSettings::setUnlimitedRecentStickers,
	});

	ender.addCollapsibleToggle({
		.id = u"ender/hideReactions"_q,
		.title = tr::ender_HideReactions(),
		.checkboxes = {
			NestedEntry{
				tr::ender_HideReactionsInChannels(tr::now),
				[] { return !EnderSettings::getInstance().showChannelReactions(); },
				[](bool v) { EnderSettings::getInstance().setShowChannelReactions(!v); }
			},
			NestedEntry{
				tr::ender_HideReactionsInGroups(tr::now),
				[] { return !EnderSettings::getInstance().showGroupReactions(); },
				[](bool v) { EnderSettings::getInstance().setShowGroupReactions(!v); }
			},
			NestedEntry{
				tr::ender_HideReactionsInPrivateChats(tr::now),
				[] { return !EnderSettings::getInstance().showPrivateChatReactions(); },
				[](bool v) { EnderSettings::getInstance().setShowPrivateChatReactions(!v); }
			}
		},
		.toggledWhenAll = false,
	});

	ender.addSectionDivider();
}

void BuildGroupsAndChannels(SectionBuilder &builder, EnderSectionBuilder &ender) {
	auto *settings = &EnderSettings::getInstance();

	builder.addSubsectionTitle(tr::lng_premium_double_limits_subtitle_channels());

	ender.addChooseButton({
		.id = u"ender/channelBottomButton"_q,
		.altIds = { u"ender/bottomButton"_q },
		.title = tr::ender_ChannelBottomButton(),
		.boxTitle = tr::ender_ChannelBottomButton(),
		.initialSelection = static_cast<int>(settings->channelBottomButton()),
		.options = {
			tr::ender_ChannelBottomButtonHide(tr::now),
			tr::ender_ChannelBottomButtonMute(tr::now),
			tr::ender_ChannelBottomButtonDiscuss(tr::now),
		},
		.setter = [](int index) {
			EnderSettings::getInstance().setChannelBottomButton(
				static_cast<ChannelBottomButton>(index));
		},
	});

	ender.addSettingToggle({
		.id = u"ender/quickAdminShortcuts"_q,
		.title = tr::ender_QuickAdminShortcuts(),
		.getter = &EnderSettings::quickAdminShortcuts,
		.setter = &EnderSettings::setQuickAdminShortcuts,
	});
	ender.addSettingToggle({
		.id = u"ender/disableGreetingSticker"_q,
		.title = tr::ender_DisableGreetingSticker(),
		.getter = &EnderSettings::disableGreetingSticker,
		.setter = &EnderSettings::setDisableGreetingSticker,
	});
	ender.addSettingToggle({
		.id = u"ender/showMessageShot"_q,
		.title = tr::ender_SettingsShowMessageShot(),
		.getter = &EnderSettings::showMessageShot,
		.setter = &EnderSettings::setShowMessageShot,
	});

	builder.addSkip();
	builder.addDividerText(tr::ender_SettingsShowMessageShotDescription());
	builder.addSkip();
}

void BuildMarks(
		SectionBuilder &builder,
		EnderSectionBuilder &ender,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &EnderSettings::getInstance();
	const auto controller = builder.controller();

	builder.addSubsectionTitle(tr::lng_settings_messages());

	builder.add([=](const WidgetContext &ctx) -> SectionBuilder::WidgetToAdd {
		auto preview = object_ptr<MessagePreview>(ctx.container, controller);
		previewState->widget = preview.data();
		return {
			.widget = std::move(preview),
			.margin = style::margins(
				0,
				st::defaultVerticalListSkip,
				0,
				st::settingsPrivacySkipTop),
		};
	});

	ender.addSettingToggle({
		.id = u"ender/replaceBottomInfoWithIcons"_q,
		.altIds = { u"ender/replaceEditedWithIcon"_q },
		.title = tr::ender_ReplaceMarksWithIcons(),
		.getter = &EnderSettings::replaceBottomInfoWithIcons,
		.setter = &EnderSettings::setReplaceBottomInfoWithIcons,
	});

	builder.scope([&] {
		builder.addButton({
			.id = u"ender/deletedMark"_q,
			.title = tr::ender_DeletedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = EnderSettings::getInstance().deletedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::ender_DeletedMarkText(),
					settings->deletedMark(),
					QString("🧹"),
					[=](const QString &value) {
						EnderSettings::getInstance().setDeletedMark(value);
					});
				Ui::show(std::move(box));
			},
		});

		builder.addButton({
			.id = u"ender/editedMark"_q,
			.title = tr::ender_EditedMarkText(),
			.st = &st::settingsButtonNoIcon,
			.label = EnderSettings::getInstance().editedMarkValue(),
			.onClick = [=] {
				auto box = Box<EditMarkBox>(
					tr::ender_EditedMarkText(),
					settings->editedMark(),
					tr::lng_edited(tr::now),
					[=](const QString &value) {
						EnderSettings::getInstance().setEditedMark(value);
					});
				Ui::show(std::move(box));
			},
		});
	}, EnderSettings::getInstance().replaceBottomInfoWithIconsValue()
		| rpl::map([](bool v) { return !v; }));

	ender.addSettingToggle({
		.id = u"ender/removeMessageTail"_q,
		.title = tr::ender_RemoveMessageTail(),
		.getter = &EnderSettings::removeMessageTail,
		.setter = &EnderSettings::setRemoveMessageTail,
	});

	ender.addSettingToggle({
		.id = u"ender/hideFastShare"_q,
		.altIds = { u"ender/hideShareButton"_q },
		.title = tr::ender_HideShareButton(),
		.getter = &EnderSettings::hideFastShare,
		.setter = &EnderSettings::setHideFastShare,
	});
	ender.addSettingToggle({
		.id = u"ender/simpleQuotesAndReplies"_q,
		.altIds = { u"ender/disableColorfulReplies"_q, u"ender/replyElements"_q },
		.title = tr::ender_SimpleQuotesAndReplies(),
		.getter = &EnderSettings::simpleQuotesAndReplies,
		.setter = &EnderSettings::setSimpleQuotesAndReplies,
	});

	const auto semiTransparent = ender.addSettingToggle({
		.id = u"ender/semiTransparentDeletedMessages"_q,
		.altIds = { u"ender/translucentDeletedMessages"_q },
		.title = tr::ender_SemiTransparentDeletedMessages(),
		.getter = &EnderSettings::semiTransparentDeletedMessages,
		.setter = &EnderSettings::setSemiTransparentDeletedMessages,
	});
	if (semiTransparent) {
		ender.addBetaBadge(semiTransparent);
	}

	ender.addSectionDivider();
}

void BuildWideMessagesMultiplier(
		SectionBuilder &builder,
		EnderSectionBuilder &ender,
		std::shared_ptr<PreviewState> previewState) {
	auto *settings = &EnderSettings::getInstance();

	constexpr auto kMinSize = 1.00;
	constexpr auto kStep = 0.05;

	const auto valueToIndex = [=](double value) {
		return static_cast<int>(std::round((value - kMinSize) / kStep));
	};

	const auto controller = builder.controller();
	ender.addSlider({
		.id = u"ender/messageBubbleRadius"_q,
		.title = tr::ender_MessageBubbleRadius(),
		.steps = 17,
		.current = settings->messageBubbleRadius(),
		.indexToValue = [](int index) { return index; },
		.onChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
		},
		.onFinalChanged = [=](int index) {
			if (previewState->widget) {
				previewState->widget->setBubbleRadius(index);
			}
			EnderSettings::getInstance().setMessageBubbleRadius(index);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [](int index) {
			return QString::number(index);
		},
	});

	ender.addSectionDivider();

	ender.addSlider({
		.id = u"ender/wideMultiplier"_q,
		.title = tr::ender_SettingsWideMultiplier(),
		.steps = 61, // (4.00 - 1.00) / 0.05 + 1
		.current = valueToIndex(settings->wideMultiplier()),
		.indexToValue = [](int index) { return index; },
		.onChanged = nullptr,
		.onFinalChanged = [=](int index) {
			EnderSettings::getInstance().setWideMultiplier(
				kMinSize + index * kStep);
			ShowRestartPrompt(controller);
		},
		.formatLabel = [=](int index) {
			return QString::number(kMinSize + index * kStep, 'f', 2);
		},
	});

	builder.addSkip();
	builder.addDividerText(tr::ender_SettingsWideMultiplierDescription());
	builder.addSkip();
}

void BuildContextMenuElements(SectionBuilder &builder, EnderSectionBuilder &ender) {
	auto *settings = &EnderSettings::getInstance();

	builder.addSubsectionTitle(tr::ender_ContextMenuElementsHeader());

	const auto options = std::vector{
		tr::ender_SettingsContextMenuItemHidden(tr::now),
		tr::ender_SettingsContextMenuItemShown(tr::now),
		tr::ender_SettingsContextMenuItemExtended(tr::now),
	};

	ender.addChooseButton({
		.id = u"ender/showReactionsPanelInContextMenu"_q,
		.title = tr::ender_SettingsContextMenuReactionsPanel(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showReactionsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowReactionsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconReactions },
	});
	ender.addChooseButton({
		.id = u"ender/showViewsPanelInContextMenu"_q,
		.title = tr::ender_SettingsContextMenuViewsPanel(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showViewsPanelInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowViewsPanelInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconShowInChat },
	});
	ender.addChooseButton({
		.id = u"ender/showHideMessageInContextMenu"_q,
		.title = tr::ender_ContextHideMessage(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showHideMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowHideMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconClear },
	});
	ender.addChooseButton({
		.id = u"ender/showUserMessagesInContextMenu"_q,
		.title = tr::ender_UserMessagesMenuText(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showUserMessagesInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowUserMessagesInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconTTL },
	});
	ender.addChooseButton({
		.id = u"ender/showMessageDetailsInContextMenu"_q,
		.title = tr::ender_MessageDetailsPC(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showMessageDetailsInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowMessageDetailsInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::menuIconInfo },
	});
	ender.addChooseButton({
		.id = u"ender/showRepeatMessageInContextMenu"_q,
		.title = tr::ender_RepeatMessage(),
		.boxTitle = tr::ender_SettingsContextMenuTitle(),
		.initialSelection = static_cast<int>(settings->showRepeatMessageInContextMenu()),
		.options = options,
		.setter = [](int i) { EnderSettings::getInstance().setShowRepeatMessageInContextMenu(static_cast<ContextMenuVisibility>(i)); },
		.icon = { &st::enderRepeatMenuIcon },
	});
	if (settings->filtersEnabled()) {
		ender.addChooseButton({
			.id = u"ender/showAddFilterInContextMenu"_q,
			.title = tr::ender_RegexFilterQuickAdd(),
			.boxTitle = tr::ender_SettingsContextMenuTitle(),
			.initialSelection = static_cast<int>(settings->showAddFilterInContextMenu()),
			.options = options,
			.setter = [](int i) { EnderSettings::getInstance().setShowAddFilterInContextMenu(static_cast<ContextMenuVisibility>(i)); },
			.icon = { &st::menuIconAddToFolder },
		});
	}

	builder.addSkip();
	builder.addDividerText(tr::ender_SettingsContextMenuDescription());
	builder.addSkip();
}

void BuildMessageFieldElements(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::ender_MessageFieldElementsHeader());

	ender.addSettingToggle({
		.id = u"ender/showAttachButtonInMessageField"_q,
		.title = tr::ender_MessageFieldElementAttach(),
		.getter = &EnderSettings::showAttachButtonInMessageField,
		.setter = &EnderSettings::setShowAttachButtonInMessageField,
		.icon = { &st::messageFieldAttachIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showCommandsButtonInMessageField"_q,
		.title = tr::ender_MessageFieldElementCommands(),
		.getter = &EnderSettings::showCommandsButtonInMessageField,
		.setter = &EnderSettings::setShowCommandsButtonInMessageField,
		.icon = { &st::messageFieldCommandsIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showAutoDeleteButtonInMessageField"_q,
		.title = tr::ender_MessageFieldElementTTL(),
		.getter = &EnderSettings::showAutoDeleteButtonInMessageField,
		.setter = &EnderSettings::setShowAutoDeleteButtonInMessageField,
		.icon = { &st::messageFieldTTLIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showEmojiButtonInMessageField"_q,
		.title = tr::ender_MessageFieldElementEmoji(),
		.getter = &EnderSettings::showEmojiButtonInMessageField,
		.setter = &EnderSettings::setShowEmojiButtonInMessageField,
		.icon = { &st::messageFieldEmojiIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showMicrophoneButtonInMessageField"_q,
		.title = tr::ender_MessageFieldElementVoice(),
		.getter = &EnderSettings::showMicrophoneButtonInMessageField,
		.setter = &EnderSettings::setShowMicrophoneButtonInMessageField,
		.icon = { &st::messageFieldVoiceIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showGiftButtonInMessageField"_q,
		.title = tr::lng_profile_action_short_gift(),
		.getter = &EnderSettings::showGiftButtonInMessageField,
		.setter = &EnderSettings::setShowGiftButtonInMessageField,
		.icon = { &st::settingsButtonIconGift },
	});
	ender.addSettingToggle({
		.id = u"ender/showAiEditorButtonInMessageField"_q,
		.title = tr::lng_ai_compose_title(),
		.getter = &EnderSettings::showAiEditorButtonInMessageField,
		.setter = &EnderSettings::setShowAiEditorButtonInMessageField,
		.icon = { &st::messageFieldCocoonAiIcon },
	});

	ender.addSectionDivider();
}

void BuildMessageFieldPopups(SectionBuilder &builder, EnderSectionBuilder &ender) {
	builder.addSubsectionTitle(tr::ender_MessageFieldPopupsHeader());

	ender.addSettingToggle({
		.id = u"ender/showAttachPopup"_q,
		.title = tr::ender_MessageFieldElementAttach(),
		.getter = &EnderSettings::showAttachPopup,
		.setter = &EnderSettings::setShowAttachPopup,
		.icon = { &st::messageFieldAttachIcon },
	});
	ender.addSettingToggle({
		.id = u"ender/showEmojiPopup"_q,
		.title = tr::ender_MessageFieldElementEmoji(),
		.getter = &EnderSettings::showEmojiPopup,
		.setter = &EnderSettings::setShowEmojiPopup,
		.icon = { &st::messageFieldEmojiIcon },
	});
}

const auto kMeta = BuildHelper({
	.id = EnderChats::Id(),
	.parentId = EnderMain::Id(),
	.title = &tr::ender_CategoryChats,
	.icon = &st::menuIconChatBubble,
}, [](SectionBuilder &builder) {
	auto ender = EnderSectionBuilder(builder);
	const auto previewState = std::make_shared<PreviewState>();

	builder.addSkip();
	BuildStickersAndEmoji(builder, ender);
	BuildGroupsAndChannels(builder, ender);
	BuildMarks(builder, ender, previewState);
	BuildWideMessagesMultiplier(builder, ender, previewState);
	BuildContextMenuElements(builder, ender);
	BuildMessageFieldElements(builder, ender);
	BuildMessageFieldPopups(builder, ender);
	builder.addSkip();
});

} // namespace

rpl::producer<QString> EnderChats::title() {
	return tr::ender_CategoryChats();
}

EnderChats::EnderChats(
	QWidget *parent,
	not_null<Window::SessionController*> controller)
: Section(parent, controller) {
	setupContent();
}

void EnderChats::setupContent() {
	const auto content = Ui::CreateChild<Ui::VerticalLayout>(this);
	build(content, kMeta.build);
	Ui::ResizeFitChild(this, content);
}

Type EnderChatsId() {
	return EnderChats::Id();
}

} // namespace Settings
