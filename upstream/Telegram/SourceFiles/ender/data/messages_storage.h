// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "ender/data/entities.h"

namespace EnderMessages {

void addEditedMessage(not_null<HistoryItem *> item);
std::vector<EnderMessageBase> getEditedMessages(not_null<HistoryItem*> item, ID minId, ID maxId, int totalLimit);
bool hasRevisions(not_null<HistoryItem*> item);

void addDeletedMessage(not_null<HistoryItem*> item);
std::vector<EnderMessageBase> getDeletedMessages(not_null<PeerData*> peer, ID topicId, ID minId, ID maxId, int totalLimit, const QString &searchQuery = QString());
bool hasDeletedMessages(not_null<PeerData*> peer, ID topicId);
void removeDeletedMessage(not_null<HistoryItem*> item);
void clearDeletedMessages(not_null<PeerData*> peer, ID topicId);

}
