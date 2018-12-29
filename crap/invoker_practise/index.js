var spells      = [],
    spell_names = {
  'eee': 'Sun Strike',
  'eeq': 'Forge Spirit',
  'eew': 'Chaos Meteor',
  'eqq': 'Ice Wall',
  'eqw': 'Deafening Blast',
  'eww': 'Alacrity',
  'qqq': 'Cold Snap',
  'qqw': 'Ghost Walk',
  'qww': 'Tornado',
  'www': 'EMP'};

update_spells = () => {
  if (spells.length > 3)
    spells.shift();

  $('.active').empty();
  spells.forEach((e) => {
    $('.active').append($('<div/>', {
      class: 'active_skill',
      id: 'active_' + e
    }));
  })
}

invoke = () => {
  var spell = spells.concat().sort().join("");
  var cur_first_spell = $('#skill_1').css('background-image');
  if (cur_first_spell != "none") {
    var spell_code = cur_first_spell.split('/').pop().split('.')[0];
    if (spell_code != spell) {
      $('#skill_2').css('background-image', 'url(' + cur_first_spell.match(/url\("(.*)"\)/)[1] + ')');
      $('#skill_name_2').text(spell_names[spell_code]);
    }
  }
  $('#skill_1').css('background-image', 'url("res/' + spell + '.png")');
  $('#skill_name_1').text(spell_names[spell]);
}

$(document).ready(() => {
  $(document).bind('keydown.q', (e) => { $('#q_cover').show(); });
  $(document).bind('keyup.q',   (e) => { $('#q_cover').hide(); });
  $(document).bind('keydown.w', (e) => { $('#w_cover').show(); });
  $(document).bind('keyup.w',   (e) => { $('#w_cover').hide(); });
  $(document).bind('keydown.e', (e) => { $('#e_cover').show(); });
  $(document).bind('keyup.e',   (e) => { $('#e_cover').hide(); });
  $(document).bind('keydown.r', (e) => { $('#invoke_cover').show(); });
  $(document).bind('keyup.r',   (e) => { $('#invoke_cover').hide(); });

  $(document).bind('keypress.q', (e) => {
    spells.push('q');
    update_spells();
  });
  $(document).bind('keypress.w', (e) => {
    spells.push('w');
    update_spells();
  });
  $(document).bind('keypress.e', (e) => {
    spells.push('e');
    update_spells();
  });
  $(document).bind('keypress.r', (e) => {
    invoke();
  });

  $('.cheatsheet').on('click', (e) => {
    $('.cheatsheet').remove();
  });
})
