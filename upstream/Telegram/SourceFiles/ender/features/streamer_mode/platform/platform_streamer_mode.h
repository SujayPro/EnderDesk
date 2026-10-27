// This is the source code of EnderGram for Desktop.
//
// We do not and cannot prevent the use of our code,
// but be respectful and credit the original author.
//
// Copyright @Radolyn, 2026
#pragma once

#include "base/basic_types.h"

class QWidget;

namespace EnderFeatures::StreamerMode::Platform {

void SetWindowCaptureExcluded(
	not_null<QWidget*> widget,
	bool excluded);

} // namespace EnderFeatures::StreamerMode::Platform
