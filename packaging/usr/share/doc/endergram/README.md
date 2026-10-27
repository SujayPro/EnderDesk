# EnderGram 1.0.0

EnderGram is a Telegram-compatible desktop client for Kubuntu/KDE Plasma
Wayland and X11, focused on low memory use and deep customisation.

Project: https://github.com/SujayPro/EnderDesk  
Channel: https://t.me/EnderGramNews  
Chat: https://t.me/EnderGramChat

## Low-memory mode

The default desktop launch targets **under 250 MB RSS** by using the
XWayland backend with the OpenGL integration disabled. The core Telegram
engine, networking and chat features still work; GPU-accelerated media
views and calls are the trade-off.

For full hardware acceleration:

```bash
endergram --gpu
# or
ENDERGRAM_GPU=1 endergram
```

Measured on the reference Kubuntu Wayland / AMD system at idle login:

- Default: **249 MB RSS / 224 MB PSS**
- `--gpu`: **~320 MB RSS / ~242 MB PSS**

The first launch can be around 265 MB while the local cache is generated; a
normal second launch settles around 248 MB.

Memory depends on open chats, media and account state.

## Launcher

The installed `/usr/bin/endergram` entry point is a small shell launcher. It
sets the low-memory profile, manages the KWin frameless rule and executes the
engine.


## Layout

- `/usr/bin/endergram` - shell launcher
- `/usr/lib/endergram/EnderGram` - patched Telegram-compatible engine
- `/usr/share/applications/com.ayugram.desktop.desktop` - menu entry
- `/usr/share/icons/hicolor/*/apps/com.endergram.desktop.png` - icons

User data lives under `~/.local/share/EnderGram`. The first launch renames the
old `AyuGramDesktop` profile/cache so the login and chats stay.


## Design

EnderGram Noir is the default dark theme: near-black surfaces, thin zinc
borders and an indigo accent (#556af3 / #2e1b9c) inspired by the public
Command Code design language. The theme file is also installed at
`/usr/share/endergram/themes/EnderGram Noir.tdesktop-theme`.

This is a palette interpretation only. No Command Code artwork, logo, copy,
code, or private Figma file is redistributed.


## Frameless window

The default launch removes the KWin titlebar for EnderGram’s main window by
adding a scoped `[endergram]` rule to `~/.config/kwinrulesrc` (backup at
`kwinrulesrc.endergram.bak`). The native WhiteSur buttons are therefore gone.

Use `Alt+F4` to close, the taskbar/tray to minimize/quit, `Alt+Left-drag` to
move, and `Alt+Right-drag` to resize. To restore the titlebar for one launch:

```bash
endergram --titlebar
```

The KDE menu also has **Launch EnderGram with native titlebar**.

## Packages

- Debian/Ubuntu/Kubuntu: `.deb`
- Portable Linux (Arch, Debian, Ubuntu, Kubuntu, etc.): `.AppImage`

- Dark: `EnderGram Noir.tdesktop-theme`
- Light: `EnderGram Frost.tdesktop-theme`
