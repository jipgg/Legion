export type WindowFlags = {
    popup_menu: boolean?,
    resizable: boolean?,
    shown: boolean?,
    hidden: boolean?,
    maximized: boolean?,
    minimized: boolean?,
    fullscreen: boolean?,
    borderless: boolean?,
    allow_high_dpi: boolean?,
    always_on_top: boolean?,
}
export type RendererFlags = {
    hardware_accelerated: boolean?,
    present_vsync: boolean?,
}
export type LaunchConfiguration = {
    window_title: string,
    window_size: Vec2,
    window_flags: WindowFlags,
}

