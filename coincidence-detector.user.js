// ==UserScript==
// @name            Script Name
// @namespace       http://example.com
// @include         *
// ==/UserScript==

var coincidences = [
  "b[u|e]rg",
  "stein",
  "blatt",
  "cohen",
  "katz",
  "kaplan",
  "jew",
  "hollywood",
  "splc"
  "wall ?street"
];
var coincidences_r = [];
for (var i = 0; i < coincidences.length; i++) {
  coincidences_r.push(new RegExp("(^|\\s+)?(\\w+)?(?!\\(+)(" + coincidences[i] + ")(?!\\)+)(\\w+)?", "gim"));
  console.log(coincidences_r[i]);
}

var all = document.getElementsByTagName("body")[0].childNodes;
for (var i = 0; i < all.length; i++) {
  if (all[i].tagName != "SCRIPT"
   && all[i].tagName != "STYLE"
   && typeof(all[i].tagName) != "undefined") {
    var html = all[i].innerHTML;
    for (var j = 0; j < coincidences_r.length; j++)
      html = html.replace(coincidences_r[j], "$1((($2$3$4)))");
    all[i].innerHTML = html;
  }
}
