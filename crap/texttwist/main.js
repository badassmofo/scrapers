let words = JSON.parse(words_json);
let min_len = 5,
    max_len = 8,
    score = 0,
    reqired_score = 0,
    total_score = 0,
    n_elem = 0,
    pool,
    positions = [],
    c_word = "",
    el_w = 0,
    anim_lock = 0;

/*
 * TODO
 *  - Handle positions on resize
 *  - Add total words
 *  - SVG letters for scaling
 *  - Type letters to select?
 *  - Timer?
 */

let rnd_int = function(min, max) {
  return Math.floor(Math.random() * (max - min + 1) + min);
};

let shuffle = function(a) {
  let i = a.length, t, r;
  while (0 !== i) {
    r    = Math.floor(Math.random() * i);
    i   -= 1;
    t    = a[i];
    a[i] = a[r];
    a[r] = t;
  }
  return a;
};

let compare = function(a, b) {
  let aa = a.split("")
  for (let x = 0, len = b.length; x < len; x++) {
    let i = aa.indexOf(b.charAt(x));
    if (i === -1)
      return false;
    aa.splice(i, 1);
  }
  return true;
};

let anagrams = function(w, n) {
  let ret = [];
  for (let i = n; i >= 0; i--)
    if (compare(w, words[i]))
      ret.push(words[i]);
  return ret;
};

let reset = function() {
  positions = [];
  let range = words_ranges[rnd_int(min_len, max_len)];
  let chars = shuffle(words[rnd_int(range[0], range[1])].split("")).join("");
  pool = anagrams(chars, range[1]);
  total_score = pool.map(x => x.length).reduce(function(a, b) { return a + b });
  reqired_score = Math.floor(.3 * total_score);
  score = 0;
  console.log(chars, pool);

  $("#current_score").text("0, 0%").css({ left: '0px' });
  $("#target_score").text(reqired_score + ", 30%");
  $("#total_score").text(total_score + ", 100%");

  const phi = 1.618033988749894848204586834;
  let w = $(window).width(), h = $(window).height();
  el_w = (w - 20 * chars.length) / chars.length ;
  let el_h = (el_w * phi) - el_w;
  let offset_x = (w - (el_w * chars.length) - ((chars.length - 1) * 10)) / 2;
  let offset_y = ((h / 2) + 10) - (el_h / 2);
  for (let i = 0; i < chars.length; ++i) {
    let x = offset_x, y1 = offset_y + 10, y2 = offset_y - el_h;
    $('<div/>', { class: 'letter inactive filled', id: 'p' + i, text: chars.charAt(i).toUpperCase() }).css({ top: y1 + 'px', left: x + 'px', width: el_w + 'px', height: el_h + 'px' }).appendTo('body');
    $('<div/>', { class: 'letter blank',  id: 'a' + i }).css({ top: y2 + 'px', left: x + 'px', width: el_w + 'px', height: el_h + 'px' }).appendTo('body');
    $('<div/>', { class: 'letter blank',  id: 'a' + (i + chars.length) }).css({ top: y1 + 'px', left: x + 'px', width: el_w + 'px', height: el_h + 'px' }).appendTo('body');
    offset_x += 10 + el_w;
    positions.push([x, y1, y2]);
  }
  $(".buttons").css({ top: offset_y + el_h + 30 });
};

let animate = function(e, options, speed, callback) {
  callback = callback || function(){};
  anim_lock += 1;
  e.animate(options, speed, function() {
    anim_lock -= 1;
    callback();
  });
};

let bounce = function(e, n, d, s) {
  for (let i = 0; i < n; ++i)
    animate(e, { top: '+=' + d }, s, animate(e, { top: '-=' + d }, s));
};

let clear = function() {
  let ia_positions = Array.from($(".inactive").map(function(_) {
    return $(this).position().left;
  }));
  let v_positions = positions.filter(function(x) {
    return !ia_positions.includes(x[0]);
  });
  $(".active").each(function(i) {
    $(this).removeClass("active").addClass("inactive").animate({ top: v_positions[i][1], left: v_positions[i][0] }, 'fast');
  });
  n_elem = 0;
  c_word = "";
};

let twist = function() {
  let new_pos = shuffle(positions.slice());
  var i = 0;
  $(".inactive").each(function(i) {
    $(this).animate({ top: new_pos[i][1], left: new_pos[i][0] }, 'fast')
    i += 1;
  });
   $(".active").each(function(_) {
     $(this).removeClass("active").addClass("inactive").animate({ top: new_pos[i][1], left: new_pos[i][0] }, 'fast')
     i += 1;
   });
  n_elem = 0;
  c_word = "";
};

let check = function() {
  const i = pool.indexOf(c_word);
  if (i !== -1) {
    pool.splice(i, 1);

    score += c_word.length;
    let score_pc = (score / total_score) * 100;
    if (score >= reqired_score)
      bounce($(".flag"), 3, 20, 'fast');
    $(".progress_bar").animate({ width: score_pc + "%" }, 'fast');
    $("#current_score").text(score + ", " + Math.floor(score_pc) + "%");

    $(".active").each(function(i) {
      $(this).delay(100 * i);
      bounce($(this), 3, 20, 'fast');
    });

    console.log("found", c_word, "remaining", pool);
  } else {
    if (!$(".inactive").length)
      clear();
  }
}

$(function() {
  reset();

  $('body').on('click', '.inactive', function(e) {
    if (anim_lock > 0)
      return;

    let el = $(e.target);
    el.removeClass("inactive").addClass("active");
    animate(el, { top: positions[n_elem][2], left: positions[n_elem][0] }, 'fast', function() {
      c_word += el.text().toLowerCase();
      n_elem += 1;
      check();
    });
  });

  $('body').on('click', '.active', function(e) {
    if (anim_lock > 0)
      return;

    let ia_positions = Array.from($(".inactive").map(function(x) {
      return $(this).position().left;
    }));
    let v_positions = positions.filter(function(x) {
      return !ia_positions.includes(x[0]);
    });

    let el = $(e.target);
    el.removeClass("active").addClass("inactive");
    let left = el.position().left;
    for (let i = 0; i < positions.length; ++i) {
      if (left === positions[i][0])
        c_word = c_word.slice(0, i) + c_word.slice(i + 1);
    }
    animate(el, { top: v_positions[0][1], left: v_positions[0][0] }, 'fast');
    n_elem -= 1;
  });

  $('body').bind('keyup', function(e) {
    if (anim_lock > 0)
      return;

    e.preventDefault();
    if ((e.keyCode > 64 && e.keyCode < 91) || (e.keyCode > 96 && e.keyCode < 123)) {
      let key = String.fromCharCode(e.keyCode).toLowerCase();
      $(".inactive").each(function(i) {
        if ($(this).text().toLowerCase() === key) {
          $(this).click();
          key = "";
        }
      });
    } else if (e.keyCode == 8 || e.keyCode == 46) {
      if (e.shiftKey)
        clear();
      else {
        let sorted = $(".active").sort(function(a, b) {
          a = $(a).position().left, b = $(b).position().left;
          return (a == b ? 0 : (a > b ? 1 : -1));
        });
        if (sorted.length > 0)
          sorted[sorted.length - 1].click();
      }
    } else if (e.keyCode == 32)
      twist();
  });

  $('.progress_cont').hover(function() {
    $('.scores').animate({ bottom: '26px', opacity: 1 }, 'fast');
    $("#current_score").fadeIn('fast');
  }, function() {
    $('.scores').animate({ bottom: '20px', opacity: 0 }, 'fast');
    $("#current_score").fadeOut('fast');
  });
  $("#clear").on('click', clear);
  $("#twist").on('click', twist);
});
