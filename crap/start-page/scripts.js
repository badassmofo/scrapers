var mascot = "kuroneko.png";
var header = "GET OUT NORMIES REEEEE!";
var json   = JSON.parse(`{
	"dev": {
		"hacker news": "https://news.ycombinator.com/news",
		"github/takeiteasy": "http://github.com/takeiteasy",
		"github/dashboard": "http://github.com/dashboard",
		"github/trending": "http://github.com/trending"
	},
	"fun": {
		"youtube": "http://youtube.com",
		"twitter": "http://twitter.com",
		"twitch": "http://twitch.tv",
		"r/dota2": "https://www.reddit.com/r/DotA2/",
		"theoldreader": "http://google.com"
	},
	"server": {
		"home": "http://192.168.1.198:5000/",
		"nzbget": "http://192.168.1.198:6789/",
		"rocket.chat": "http://192.168.1.198:3000/"
	}
}`);

document.addEventListener('DOMContentLoaded', function() {
  document.getElementById("mascot").src = mascot;
  document.getElementById("title").textContent = header;

  var content = "";
  for (var k in json) {
    if (json.hasOwnProperty(k)) {
      content += '<div class="trigger">' + k + '</div><div class="toggle"><ul>';
      for (k2 in json[k])
        content += '<li><a href="' + json[k][k2] + '">' + k2 + '</a></li>';
      content += '</ul></div>'
    }
  }
  document.getElementById("items").innerHTML = content;

  document.querySelector('body').addEventListener('click', function(event) {
    if (event.target.className == "trigger") {
      event.target.nextSibling.classList.toggle('hide_toggle');
    }
  });
  document.getElementById("main_wrap").children[0].className += " load";
}, false);
