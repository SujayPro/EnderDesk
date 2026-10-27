# EnderGram

![EnderGram Лого](.github/EnderGram.png) ![EnderChan](.github/EnderChan.png)

[ [English](README.md)  | Русский ]

## Функции и Фишки

- Полный режим призрака (настраиваемый)
- История удалений и изменений сообщений
- Кастомизация шрифта
- Режим Стримера
- Локальный телеграм премиум
- Переводчик
- Превью медиа и быстрая реакция при сильном нажатии на тачпад (macOS)
- Улучшенный вид

И многое другое. Посмотрите нашу [Документацию](https://github.com/SujayPro/EnderGram/desktop/) для более подробной информации.

<h3>
  <details>
    <summary>Превью</summary>
    <table>
      <tr>
        <td><img src='.github/demos/demo1.png' width='268' alt='Preferences'></td>
        <td><img src='.github/demos/demo2.png' width='268' alt='EnderGram Options'></td>
        <td><img src='.github/demos/demo3.png' width='268' alt='Message Filters'></td>
      </tr>
      <tr>
        <td><img src='.github/demos/demo4.png' width='268' alt='Appearance'></td>
        <td><img src='.github/demos/demo5.png' width='268' alt='Chats'></td>
      </tr>
    </table>
  </details>
</h3>

## Установка

### Windows

#### Официальная версия

Вы можете скачать готовый бинарный файл со вкладки [Releases](https://github.com/SujayPro/EnderGram/releases) или из
[Телеграм канала](https://t.me/EnderGramNews).

#### Winget

```bash
winget install RadolynLabs.EnderGramDesktop
```

#### Scoop

```bash
scoop bucket add extras
scoop install endergram
```

#### Сборка вручную

Следуйте [официальному руководству](https://github.com/SujayPro/EnderGram/blob/dev/docs/building-win-x64.md), если
вы хотите собрать EnderGram сами.

### macOS

#### Официальная версия

Вы можете скачать подписанный пакет со вкладки [Releases](https://github.com/SujayPro/EnderGram/releases).

#### Homebrew

```bash
brew install --cask endergram
```

### Arch Linux

#### Из исходников (рекомендованный способ)

Установите `endergram-desktop` из [AUR](https://aur.archlinux.org/packages/endergram-desktop).

#### Готовые бинарники

Установите `endergram-desktop-bin` из [AUR](https://aur.archlinux.org/packages/endergram-desktop-bin).

Примечание: данный пакет собирается не нами.

### NixOS

#### Флейк (рекомендуется)

Установите `endergram-desktop` из [ndfined-crp/endergram-desktop](https://github.com/ndfined-crp/endergram-desktop)

#### Nixpkgs

Установите `endergram-desktop` из [nixpkgs](https://search.nixos.org/packages?channel=unstable&show=endergram-desktop)

### ALT Linux

[Sisyphus](https://packages.altlinux.org/en/sisyphus/srpms/endergram-desktop/)

### Gentoo Linux

Инструкцию по установке можно найти в [этом репозитории](https://codeberg.org/OverLessArtem/endergram-ebuild-gentoo).

### Void Linux
Инструкцию по установке можно найти в [этом репозитории](https://codeberg.org/OverLessArtem/endergram-template-void)

### EPM

`epm play endergram`

### Fedora

Из репозитория [RPM Fusion](https://admin.rpmfusion.org/pkgdb/package/free/endergram-desktop/).

```bash
dnf install endergram-desktop
```

### Любой другой Линукс дистрибутив

Flatpak: https://github.com/0FL01/EnderGramDesktop-flatpak

Или следуйте [официальному руководству](https://github.com/SujayPro/EnderGram/blob/dev/docs/building-linux.md).

## Пожертвования

Вам нравится использовать **EnderGram**? Оставьте нам чаевые!

[Здесь доступные варианты.](https://github.com/SujayPro/EnderGram/donate/)

## Использованные материалы

### Телеграм клиенты

- [Telegram Desktop](https://github.com/telegramdesktop/tdesktop)
- [Kotatogram](https://github.com/kotatogram/kotatogram-desktop)
- [64Gram](https://github.com/TDesktop-x64/tdesktop)
- [Forkgram](https://github.com/forkgram/tdesktop)

### Использованные библиотеки

- [JSON for Modern C++](https://github.com/nlohmann/json)
- [SQLite](https://github.com/sqlite/sqlite)
- [sqlite_orm](https://github.com/fnc12/sqlite_orm)

### Иконки

- [Solar Icon Set](https://www.figma.com/community/file/1166831539721848736)

### Боты

- [TelegramDB](https://t.me/tgdatabase) для получения юзернейма по ID (до закрытия бесплатной версии 2 апреля 2026)
