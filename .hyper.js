module.exports = {
  config: {
    // default font size in pixels for all tabs
    fontSize: 12,

    // font family with optional fallbacks
    fontFamily: 'Menlo, "DejaVu Sans Mono", "Lucida Console", monospace',

    // terminal cursor background color and opacity (hex, rgb, hsl, hsv, hwb or cmyk)
    cursorColor: 'rgba(149, 162, 187, 0.8)',

    // `BEAM` for |, `UNDERLINE` for _, `BLOCK` for █
    cursorShape: 'BEAM',

    // color of the text
    foregroundColor: 'rgb(149, 162, 187)',

    // terminal background color
    backgroundColor: '#292C33',

    // border color (window, tabs)
    borderColor: '#292C33',

    // custom css to embed in the main window
    css: `
    .tabs_nav {
      background-color: rgb(34, 37, 42);
    }

    .tab_active {
      background-color: #292C33;
    }

    .terms_termGroup {
      background-color: #292C33;
    }
    `,

    // custom css to embed in the terminal window
    termCSS: `
    [style*="background-color: rgb(149, 162, 187)"] {
      background-color: transparent !important;
    }
    `,

    // custom padding (css format, i.e.: `top right bottom left`)
    padding: '12px 14px',

    // the full list. if you're going to provide the full color palette,
    // including the 6 x 6 color cubes and the grayscale map, just provide
    // an array here instead of a color map object
    colors: {
      black: "#292C33",
      red: "#BF6E7C",
      white: "#95A2BB",
      green: "#88B379",
      yellow: "#D9BD86",
      blue: "#66A5DF",
      magenta: "#C699C5",
      cyan: "#6EC6C6",

      lightBlack: "#484c54",
      lightRed: "#dd8494",
      lightWhite: "#adbcd7",
      lightGreen: "#9dcc8c",
      lightYellow: "#e9cc92",
      lightBlue: "#6cb2f0",
      lightMagenta: "#e8b6e7",
      lightCyan: "#7adada"
    },

    // the shell to run when spawning a new session (i.e. /usr/local/bin/fish)
    // if left empty, your system's login shell will be used by default
    shell: '/bin/zsh',

    // for setting shell arguments (i.e. for using interactive shellArgs: ['-i'])
    // by default ['--login'] will be used
    shellArgs: ['--login'],

    // for environment variables
    env: {},

    // set to false for no bell
    bell: 'SOUND',

    // if true, selected text will automatically be copied to the clipboard
    copyOnSelect: false,

    // URL to custom bell
    bellSoundURL: '/Users/rusty/.sfx/jan.mp3'

    // for advanced config flags please refer to https://hyper.is/#cfg
  },

  // a list of plugins to fetch and install from npm
  // format: [@org/]project[#version]
  // examples:
  //   `hyperpower`
  //   `@company/project`
  //   `project#1.0.1`
  plugins: [],

  // in development, you can create a directory under
  // `~/.hyper_plugins/local/` and include it here
  // to load it and avoid it being `npm install`ed
  localPlugins: []
};
