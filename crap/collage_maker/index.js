const electron = require('electron');
const app = electron.app;
const ipc = electron.ipcMain;
const dialog = electron.dialog;
const BrowserWindow = electron.BrowserWindow;
const fs = require('fs');
const path = require('path')
const url = require('url')
const { spawn } = require('child_process');

let renderer = null;
let library = [];
let win = null;

ipc.on('get-sender', function(e) {
  renderer = e.sender;
});

ipc.on('open', function(e, arr, exts) {
  let files = dialog.showOpenDialog({
    filters: [{
      name: 'Add files',
      extensions: exts
    }],
    properties: ['openFile', 'multiSelections']
  });
  if (files === undefined || files.length == 0)
    return;

  files.filter(function(f) {
    return !library.includes(f);
  }).forEach(function(f) {
    fs.readFile(f, { encoding: 'base64' }, function(err, data) {
      if (err) throw err;
      renderer.send("add", data, f);
      library.push(f);
    });
  });
});

ipc.on('save', function(e, objs) {
  if (objs.length == 0) {
    renderer.send("result", false);
    return;
  }

  let out = dialog.showSaveDialog({ defaultPath: 'spritesheet.png' });
  if (out === undefined) {
    renderer.send("result", false);
    return;
  }

       console.log(JSON.stringify(objs))
  const proc = spawn(path.join(__dirname, 'make_image'), [out, objs.length, JSON.stringify(objs)]);
  proc.stdout.on('data', (data) => {
    process.stdout.write("[MAKE_IMAGE] STDOUT: " + data.toString());
  });
  proc.stderr.on('data', (data) => {
    process.stderr.write("[MAKE_IMAGE] STDERR: " + data.toString());
  });
  proc.on('exit', (code) => {
    renderer.send("result", (code === 0));
  });
});

app.on('ready', function() {
  win = new BrowserWindow({
    width: 800,
    height: 600,
    minWidth: 640,
    minHeight: 480
  });
  win.webContents.openDevTools();
  win.loadURL(url.format({
    pathname: path.join(__dirname, 'www/index.html'),
    protocol: 'file:',
    slashes: true
  }));
  win.on('closed', function () {
    app.quit();
  });
});

app.on('window-all-closed', function() {
  app.quit();
});
