module.exports = {
  config: {
    fontSize: 12,
    fontFamily: 'Menlo, "DejaVu Sans Mono", "Lucida Console", monospace',
    cursorColor: 'rgba(149, 162, 187, 0.8)',
    cursorShape: 'BEAM',
    foregroundColor: 'rgb(149, 162, 187)',
    backgroundColor: 'rgb(41, 44, 51)',
    borderColor: 'rgb(41, 44, 51)',
    css: `
    .tabs_nav {
      background-color: rgb(34, 37, 42);
    }
    .tab_active {
      background-color: rgb(41, 44, 51);
    }
    .terms_termGroup {
      background-color: rgb(41, 44, 51);
    }
    .splitpane_divider {
      background-color: rgb(149, 162, 187) !important;
    }
    `,
    termCSS: `
    [style*="background-color: rgb(149, 162, 187)"] {
      background-color: transparent !important;
    }
    `,
    padding: '12px 14px',
    colors: {
      black:        "#292C33",
      red:          "#BF6E7C",
      white:        "#95A2BB",
      green:        "#88B379",
      yellow:       "#D9BD86",
      blue:         "#66A5DF",
      magenta:      "#C699C5",
      cyan:         "#6EC6C6",
      lightBlack:   "#484c54",
      lightRed:     "#dd8494",
      lightWhite:   "#adbcd7",
      lightGreen:   "#9dcc8c",
      lightYellow:  "#e9cc92",
      lightBlue:    "#6cb2f0",
      lightMagenta: "#e8b6e7",
      lightCyan:    "#7adada"
    },
    shell: '/bin/zsh',
    shellArgs: ['--login'],
    env: {},
    bell: 'SOUND',
    copyOnSelect: false,
    bellSoundURL: ''
  },
  plugins: [],
  localPlugins: []
};
