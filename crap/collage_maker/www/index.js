const ipc = require('electron').ipcRenderer;
const webFrame = require('electron').webFrame;

let canvas = null;
let canvas_component = null;
let grid_size = 70;
let world_pos = new fabric.Point(0, 0);
let world_pos_grid = new fabric.Point(0, 0);
let grab_pos = new fabric.Point(0, 0);
let is_dragging = false;
let snap_to_grid = false;
let saving = false;

Number.prototype.round_factor = function(n) {
  if (this > 0)
    return Math.ceil(this / n) * n;
  else if (this < 0)
    return Math.floor(this / n) * n;
  else
    return n;
};

Number.prototype.clamp = function(min, max) {
  return Math.min(Math.max(this, min), max);
};

guid_s4 = function() {
  return Math.floor((1 + Math.random()) * 0x10000).toString(16).substring(1);
}

guid = function() {
  return guid_s4() + guid_s4() + '-' + guid_s4() + '-' + guid_s4() + '-' + guid_s4() + '-' + guid_s4() + guid_s4() + guid_s4();
};

adjust_for_zoom = function(n) {
  return n * (1 / canvas.getZoom());
};

canvas_top_left = function() {
  return new fabric.Point(adjust_for_zoom(-canvas.viewportTransform[4]), adjust_for_zoom(-canvas.viewportTransform[5]));
};

canvas_width_height = function() {
  return new fabric.Point(adjust_for_zoom(canvas_component.width()), adjust_for_zoom(canvas_component.height()));
};

resize_canvas = function() {
  canvas.setWidth(canvas_component.width());
  canvas.setHeight(canvas_component.height());
};

ipc.on("add", function(e, data, f) {
  $(".siderbar_inner").append("<div class='library_file' style='background-image: url(data:image/" + (f.split('.').pop()) + ";base64," + data + ");'><div class='library_file_header'><div class='marquee'>" + f + "</div></div></div>");
});

ipc.on("result", function(e, success) {
	if (success)
		alertify.success("Saving complete!");
	else
		alertify.error("Failed to save spritesheet!")
	saving = false;
});

document.addEventListener("contextmenu", (event) => event.preventDefault());
document.addEventListener("DOMContentLoaded", () => {
  ipc.send('get-sender');
  webFrame.setZoomLevelLimits(1, 1);

  canvas = new fabric.Canvas('render');
  canvas.enableRetinaScaling = true;
  canvas.imageSmoothingEnabled = true;
  canvas.selection = false;
  canvas.selectionColor = "rgba(56, 121, 217, 0.3)";
  canvas_component = $('#render').parent().parent();
  canvas.absolutePan(new fabric.Point(-canvas_component.width() / 2, -canvas_component.height() / 2));

  $(window).on('resize', function() {
    resize_canvas();
  }).trigger('resize');

  $(document).bind('keydown', 'alt', function() {
    canvas.selection = true;
  });

  $(document).bind('keyup', 'alt', function() {
    canvas.selection = false;
  });

  $(document).bind('keydown', 'ctrl', function() {
    snap_to_grid = false;
  });

  $(document).bind('keyup', 'ctrl', function() {
    snap_to_grid = true
  });

  $(document).bind('keydown', 'alt+z', function() {
    canvas.absolutePan(new fabric.Point(-canvas_component.width() / 2, -canvas_component.height() / 2));
  });

  $(document).bind('keydown', 'ctrl+s', function() {
		if (!saving) {
			ipc.send("save", canvas.getObjects("image").filter(function(o) {
	      return !o.dirty;
	    }).map(function(o) {
             console.log(o);
	      return {
	        'width': o.width * o.scaleX,
	        'height': o.height * o.scaleY,
	        'x': Math.floor(o.left),
	        'y': Math.floor(o.top),
          'fx': o.flipX,
          'fy': o.flipY,
	        'src': $(o._element).attr("alt")
	      }
	    }));
			saving = true;
		}
  });

  canvas.on('mouse:move', function(e) {
    var world_pos_grid, world_pos_zoom;
    world_pos.x = Math.ceil(-canvas.viewportTransform[4] + (e.e.clientX - canvas_component.offset().left)) - 2;
    world_pos.y = Math.ceil(-canvas.viewportTransform[5] + (e.e.clientY - canvas_component.offset().top)) - 2;
    if (is_dragging) {
      canvas.absolutePan(new fabric.Point(-canvas.viewportTransform[4] + -world_pos.x + grab_pos.x, -canvas.viewportTransform[5] + -world_pos.y + grab_pos.y));
    } else {
      world_pos_zoom = new fabric.Point(Math.floor(adjust_for_zoom(world_pos.x)), Math.floor(adjust_for_zoom(world_pos.y)));
      world_pos_grid = new fabric.Point((world_pos_zoom.x === 0 ? 0 : world_pos_zoom.x.round_factor(grid_size) / grid_size), (world_pos_zoom.y === 0 ? 0 : -(world_pos_zoom.y.round_factor(grid_size) / grid_size)));
      $('#editor_pos').text(("x:" + world_pos_zoom.x + ", y:" + (world_pos_zoom.y)) + " @ [" + world_pos_grid.x + ", " + world_pos_grid.y + "]");
    }
    $("#command").width(window.innerWidth - $('#editor_pos').width() - 50);
  });

  canvas.on('mouse:down', function(e) {
    if (!canvas.selection && !canvas.getActiveObject() && !canvas.getActiveGroup()) {
      is_dragging = true;
      grab_pos.x = Math.ceil(-canvas.viewportTransform[4] + (e.e.clientX - canvas_component.offset().left)) - 2;
      grab_pos.y = Math.ceil(-canvas.viewportTransform[5] + (e.e.clientY - canvas_component.offset().top)) - 2;
    }
  });

  canvas.on('mouse:up', function(e) {
    is_dragging = false;
  });

  canvas.on('mouse:out', function(e) {
    if (!is_dragging) {
      return $('#editor_pos').text("x:-, y:- @ [-, -]");
    }
  });

  canvas_component.on('mousewheel', function(e) {
    e.preventDefault();
    e.stopImmediatePropagation();
    if (e.originalEvent.altKey)
      canvas.setZoom((canvas.getZoom() + e.deltaY * 0.01).clamp(0.5, 2));
  });

  $('#add_button').on('click', function(e) {
    ipc.send('open', ['png', 'jpg', 'jpeg', 'bmp']);
  });

  dragula([document.querySelector('.siderbar_inner'), document.querySelector('.canvas-container')], {
    copy: true,
    ignoreInputTextSelection: true
  }).on('drop', function(el) {
    let el_src = $(el).css('background-image').replace(/^url\(\"?/g, '').replace(/\"?\)$/, '');
    if (el_src !== "") {
      let id = guid();
      $('.hidden_images').append($('<img>', {
        id: id,
        src: el_src,
        alt: $(el).text()
      }));
      var e = window.event;
      let img = new fabric.Image(document.getElementById(id), {
        left: Math.ceil(-canvas.viewportTransform[4] + (e.clientX - canvas_component.offset().left)) - 2,
        top: Math.ceil(-canvas.viewportTransform[5] + (e.clientY - canvas_component.offset().top)) - 2,
        angle: 0,
        opacity: 1
      });
      img.lockRotation = true;
      img.hasRotatingPoint = false;
      canvas.add(img);
      el.remove();
    }
  });

  canvas.on('object:moving', function(options) {
    if (snap_to_grid) {
      options.target.set({
        left: Math.round(options.target.left / grid_size) * grid_size,
        top: Math.round(options.target.top / grid_size) * grid_size
      });
    }
    canvas.bringToFront(options.target)
  });

  canvas.add(new fabric.Line([-10, 0, 10, 0], {
    stroke: "#c6c6c6",
    strokeWidth: 1,
    hoverCursor: 'default',
    selectable: false
  }));

  canvas.add(new fabric.Line([0, -10, 0, 10], {
    stroke: "#c6c6c6",
    strokeWidth: 1,
    hoverCursor: 'default',
    selectable: false
  }));
}, false);
