Array.prototype.contains = function (needle) {
  for (i in this)
    if (this[i] == needle)
      return true;
  return false;
}

String.prototype.capitalise = function () {
  return this.charAt(0).toUpperCase() + this.slice(1).toLowerCase();
}

function add_commas(n) {
    return n.toString().replace(/\B(?=(\d{3})+(?!\d))/g, ",");
}

function add_commas_cr(n) {
  return add_commas(n.slice(0, n.length - 2)) + "cr";
}

function k_to_0(cr) {
  if ((cr[cr.length - 1] === 'K'))
    return add_commas(cr.slice(0, cr.length - 1) + "000") + "cr";
  return cr;
}

function update_planet_outbreak_count(planet) {
  $("#" + planet + ".trigger").text(planet + " (" + $("ul#" + planet).children().length + ")")
}

var current_alerts    = [],
    current_outbreaks = [];

function update() {
  $.ajax({
    type:     "GET",
    url:      "alerts.php",
    dataType: "xml",
    success: function (xml) {
      if (!xml) return;

      var new_alerts = [];
      $(xml).find("item").each(function () {
        var guid = $(this).find('guid').text();
        new_alerts.push(guid);
        if (!current_alerts.contains(guid)) {
          current_alerts.push(guid);

          var type  = $(this).find("author").text();
          var title = $(this).find("title").text();
          if (type === "Alert") {
            var expire  = new Date($(this).find("wfexpiry").text());
            if ((new Date()) > expire)
              return;

            var m         = (/^(.* - )?(\d+cr) - (.*) \((\S+)\) - (\d+)m$/g).exec(title),
                reward  = (m[1] ? m[1].substr(0, m[1].length - 3) + " + " : ""),
                credits =  add_commas_cr(m[2]),
                node    =  m[3],
                planet  =  m[4],
                desc    = $(this).find("description").text(),
                fact    = $(this).find("wffaction").text().slice(3).capitalise();

            $("#" + type).prepend("<div class='trigger' id='" + guid + "'><div class='aleft'><center>" + node + " (" + planet + ")</center></div><div class='aright'><center>" + reward + credits + "</center></div></div><div class='toggle' id='" + guid + "'><div class='aleft'>" + fact + " - " + desc +  "</div><div class='aright'><div class='clock' id='" + guid + "'><div class='time' id='" + guid + "'></div></div></div></div>");

            $("#" + guid + ".clock").countdown(expire, function(event) {
              $("#" + guid + ".time").html(event.strftime('%M:%S remaining'));
            }).on('finish.countdown', function() {
              $("#" + guid + ".trigger").remove();
              $("#" + guid +  ".toggle").remove();
            });
          } else if (type === "Invasion") {
            var m         = (/^(([Corpus|Grineer]+) \((.*)\)) VS\. (([Corpus|Grineer]+) \((.*)\)) - (.*) \((\S+)\)$/g).exec(title),
                f1        = m[2],
                f1_reward = k_to_0(m[3]),
                f2        = m[5],
                f2_reward = k_to_0(m[6]),
                node      = m[7],
                planet    = m[8];

            $("#Invasion").append("<div class='trigger center-it' id='" + guid + "'>" + node + " (" + planet + ")</div><div class='toggle' id='" + guid + "'><div class='afarleft'>" + f1 + "<br/>" + f1_reward + "</div><div class='acentre'>VS.</div><div class='afarright'>" + f2 + "<br/>" + f2_reward + "</div></div>")
          } else if (type === "Outbreak") {
            var m      = (/^(.*) - (PHORID SPAWN )?(.*) \((.*)\)$/g).exec(title),
                reward = ((/^\d+cr$/g).test(m[1]) ? add_commas_cr(m[1]) : m[1]),
                phorid = (typeof m[2] === 'undefined' ? "" : " -- PHORID"),
                node   = m[3],
                planet = m[4];

            if (!current_outbreaks.contains(planet)) {
              $("#Outbreak").append("<div class='trigger center-it' id='" + planet + "'>" + planet + " (0)</div><div class='toggle' id='" + planet + "'><ul id='" + planet + "'></ul></div>");
              current_outbreaks.push(planet);
            }

            $("ul#" + planet + "").append("<li class='outbreak' id='" + guid + "'><div class='aleft'>" + node + phorid + "</div><div class='aright'>" + reward + "</div></li>");
            update_planet_outbreak_count(planet);
          }

          for (var i = 0; i < current_alerts.length; ++i) {
            if (!new_alerts.contains(current_alerts[i])) {
              $("#" + current_alerts[i] + ".trigger").remove();
              $("#" + current_alerts[i] +  ".toggle").remove();
              array.splice(i, 1);
            }
          }
        }
      });
    }
  });
}

$(document).ready(function () {
  $(document).on("click", ".trigger", function () {
    $(this).next(".toggle").stop().slideToggle("normal");
  });

  setInterval(update, 60000);
});
