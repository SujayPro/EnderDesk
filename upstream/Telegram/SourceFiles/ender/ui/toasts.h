#pragma once

#include "ui/toast/toast.h"

namespace Ender::Ui {

void ShowToastWithAction(
	::Ui::Toast::Config &&config,
	const QString &buttonText,
	Fn<void()> callback);

}
