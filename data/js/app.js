// used when hosting the site on the ESP8266
var address = location.hostname;
var urlBase = "";

// used when hosting the site somewhere other than the ESP8266 (handy for testing without waiting forever to upload to SPIFFS)
// var address = "192.168.86.36";
// var urlBase = "http://" + address + "/";

var postColorTimer = {};
var postValueTimer = {};

var ignoreColorChange = false;

// var ws = new ReconnectingWebSocket("ws://" + address + ":81/", ["arduino"]);
// ws.debug = true;

// ws.onmessage = function(evt) {
//   if (evt.data != null)
//   {
//     var data = JSON.parse(evt.data);
//     if(data == null) return;
//     updateFieldValue(data.name, data.value);
//   }
// }

var allData = {};

$(document).ready(function () {
  $("#status").html("Connecting, please wait...");

  $("#btnTop").click(function () {
    $([document.documentElement, document.body]).animate(
      {
        scrollTop: $("#top").offset().top,
      },
      1000
    );
  });

  $("#btnScrollToBottom").click(function () {
    $([document.documentElement, document.body]).animate(
      {
        // TODO fix this as accordionImportExport was removed
        scrollTop: $("#accordionImportExport").offset().top,
      },
      1000
    );
  });

  $("#btnCopy").click(function () {
    $("#textareaFields").select();
    document.execCommand("copy");
  });

  $("#btnExport").click(function () {
    const fields = allData.reduce((previous, current) => {
      if (
        current.name === "name" ||
        current.value === null ||
        current.value === undefined
      )
        return previous;
      return {
        ...previous,
        [current.name]: current.value,
      };
    }, {});
    $("#textareaFields").val(JSON.stringify(fields, null, 2));
  });

  $("#btnImport").click(function () {
    const text = $("#textareaFields").val();
    const fields = JSON.parse(text);
    Object.keys(fields).forEach((name) => {
      const newValue = fields[name];
      if (newValue === null || newValue === undefined) return;

      const field = allData.find((f) => f.name === name);
      if (!field || field.value === newValue) return;
      const oldValue = field.value;

      console.log({ name, oldValue, newValue });
      postValue(name, newValue);
      field.value = newValue;
      updateFieldValue(name, newValue);
    });
  });

  $("#btnSave").click(function () {
    var text = $("#textareaFields").val();
    var filename = $("#inputFilename").val() || "preset1.json";
    var blob = new Blob([text], { type: "application/json;charset=utf-8" });
    saveAs(blob, filename);
  });

  $("#btnOpen").click(function () {
    $("#inputFile").click();
  });

  $("#inputFile").change(function (e) {
    var files = e.target.files;
    if (files.length < 1) {
      return;
    }
    var file = files[0];
    var reader = new FileReader();
    reader.onload = onFileLoaded;
    reader.readAsText(file);
    $(this).val("");
  });

  function onFileLoaded(e) {
    const result = e.target.result;
    const json = JSON.parse(result);
    $("#textareaFields").val(JSON.stringify(json, null, 2));
  }

  $.get(urlBase + "all", function (data) {
    $("#status").html("Loading, please wait...");

    allData = data;

    $.each(data, function (index, field) {
      if (field.type == "Number") {
        addNumberField(field);
      } else if (field.type == "Boolean") {
        addBooleanField(field);
      } else if (field.type == "Select") {
        addSelectField(field);
      } else if (field.type == "Color") {
        addColorFieldPalette(field);
        addColorFieldPicker(field);
      } else if (field.type == "Section") {
        addSectionField(field);
      } else if (field.type == "String") {
        addStringField(field, false);
      } else if (field.type == "Label") {
        addStringField(field, true);
      }
    });

    $("#status").html("Ready");
  }).fail(function (errorThrown) {
    console.log(errorThrown);
  });
});

function addNumberField(field) {
  var template = $("#numberTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "input-" + field.name);
  label.text(field.label);

  var input = template.find(".input");
  var slider = template.find(".slider");
  slider.attr("id", "input-" + field.name);
  if (field.min) {
    input.attr("min", field.min);
    slider.attr("min", field.min);
  }
  if (field.max) {
    input.attr("max", field.max);
    slider.attr("max", field.max);
  }
  if (field.step) {
    input.attr("step", field.step);
    slider.attr("step", field.step);
  }
  input.val(field.value);
  slider.val(field.value);

  slider.on("change mousemove", function () {
    input.val($(this).val());
  });

  slider.on("change", function () {
    var value = $(this).val();
    input.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

  input.on("change", function () {
    var value = $(this).val();
    slider.val(value);
    field.value = value;
    delayPostValue(field.name, value);
  });

  $("#form").append(template);
}

function addBooleanField(field) {
  var template = $("#booleanTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "btn-group-" + field.name);
  label.text(field.label);

  var btngroup = template.find(".btn-group");
  btngroup.attr("id", `btn-group-${field.name}`);

  const idBtnOn = `btnOn-${field.name}`;
  const idBtnOff = `btnOff-${field.name}`;

  // set unique button ids
  var btnOn = template.find("#btnOn");
  var btnOff = template.find("#btnOff");
  btnOn.attr("id", idBtnOn);
  btnOn.attr("name", `${field.name}On`);
  btnOff.attr("id", idBtnOff);
  btnOff.attr("name", `${field.name}Off`);

  // set unique label ids
  var labelOn = template.find(`[for='btnOn']`);
  var labelOff = template.find(`[for='btnOff']`);
  labelOn.attr("for", idBtnOn);
  labelOff.attr("for", idBtnOff);

  if (field.value) {
    btnOn.prop("checked", true);
    btnOff.prop("checked", false);
  } else {
    btnOn.prop("checked", false);
    btnOff.prop("checked", true);
  }

  btnOn.click(function () {
    setBooleanFieldValue(field, btnOn, btnOff, 1);
  });
  btnOff.click(function () {
    setBooleanFieldValue(field, btnOn, btnOff, 0);
  });

  $("#form").append(template);
}

function addSelectField(field) {
  var template = $("#selectTemplate").clone();

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var id = "input-" + field.name;

  var label = template.find(".control-label");
  label.attr("for", id);
  label.text(field.label);

  var select = template.find(".form-select");
  select.attr("id", id);

  for (var i = 0; i < field.options.length; i++) {
    var optionText = field.options[i];
    var option = $("<option></option>");
    option.text(optionText);
    option.attr("value", i);
    select.append(option);
  }

  select.val(field.value);

  select.change(function () {
    var value = template.find("#" + id + " option:selected").index();
    postValue(field.name, value);
  });

  var previousButton = template.find(".btn-previous");
  var nextButton = template.find(".btn-next");

  previousButton.click(function () {
    var value = template.find("#" + id + " option:selected").index();
    var count = select.find("option").length;
    value--;
    if (value < 0) value = count - 1;
    select.val(value);
    postValue(field.name, value);
  });

  nextButton.click(function () {
    var value = template.find("#" + id + " option:selected").index();
    var count = select.find("option").length;
    value++;
    if (value >= count) value = 0;
    select.val(value);
    postValue(field.name, value);
  });

  $("#form").append(template);
}

function addColorFieldPicker(field) {
  var template = $("#colorTemplate").clone();
  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var colorInput = template.find(".form-control-color");

  colorInput.val(rgbToHex(field.value));
  colorInput.on("change", function () {
    let hex = $("#input-solidColor").val();
    let rgbValue = hexToRgb(hex);
    field.value = `${rgbValue.r},${rgbValue.g},${rgbValue.b}`;

    delayPostColor(field.name, rgbValue);
  });
  $("#form").append(template);
}

function addColorFieldPalette(field) {
  var template = $("#colorPaletteTemplate").clone();

  var buttons = template.find(".btn-color");

  var label = template.find(".control-label");
  label.text(field.label);

  buttons.each(function (index, button) {
    $(button).click(function () {
      var rgb = $(this).css("backgroundColor");
      var components = rgbToComponents(rgb);

      field.value = components.r + "," + components.g + "," + components.b;
      postColor(field.name, components);
      $(`#input-${field.name}`).val(rgbToHex(field.value));
    });
  });

  $("#form").append(template);
}

function addSectionField(field) {
  var template = $("#sectionTemplate").clone();

  template.attr("id", "form-group-section-" + field.name);
  template.attr("data-field-type", field.type);

  $("#form").append(template);
}

function addStringField(field, readonly) {
  var template;

  if (readonly) {
    template = $("#labelTemplate").clone();
  } else {
    template = $("#stringTemplate").clone();
  }

  template.attr("id", "form-group-" + field.name);
  template.attr("data-field-type", field.type);

  var label = template.find(".control-label");
  label.attr("for", "input-" + field.name);
  label.text(field.label);

  var input = template.find(".input");
  input.val(field.value);

  if (!readonly) {
    input.on("change", function () {
      var value = $(this).val();
      field.value = value;
      delayPostValue(field.name, value);
    });
  }

  $("#form").append(template);
}

function updateFieldValue(name, value) {
  var group = $("#form-group-" + name);

  var type = group.attr("data-field-type");

  if (type == "Number") {
    var input = group.find(".form-control");
    input.val(value);
  } else if (type == "Boolean") {
    const btnOn = group.find("#btnOn-" + name);
    const btnOff = group.find("#btnOff-" + name);

    btnOn.attr("class", value ? "btn btn-outline-primary" : "btn btn-primary");
    btnOff.attr(
      "class",
      !value ? "btn btn-outline-primary" : "btn btn-primary"
    );
  } else if (type == "Select") {
    var select = group.find(".form-control");
    select.val(value);
  } else if (type == "Color") {
    var input = group.find(".form-control");
    input.val("rgb(" + value + ")");
  }
}

function setBooleanFieldValue(field, btnOn, btnOff, value) {
  field.value = value;

  if (field.value) {
    btnOn.prop("checked", true);
    btnOff.prop("checked", false);
  } else {
    btnOn.prop("checked", false);
    btnOff.prop("checked", true);
  }

  postValue(field.name, field.value);
}

function postValue(name, value) {
  $("#status").html("Setting " + name + ": " + value + ", please wait...");

  var body = { name: name, value: value };

  // TODO: hack for new fields
  const oldFieldNames = [
    "power",
    "cooling",
    "sparking",
    "speed",
    "twinkleSpeed",
    "twinkleDensity",
    "solidColor",
    "pattern",
    "patternName",
    "theme",
    "themeName",
    "brightness",
    "autoplay",
    "autoplayDuration",
    "showClock",
    "clockBackgroundFade",
  ];

  if (oldFieldNames.some((f) => f === name)) {
    $.post(urlBase + name + "?value=" + value, body, function (data) {
      if (data.name != null) {
        $("#status").html("Set " + name + ": " + data.name);
      } else {
        $("#status").html("Set " + name + ": " + data);
      }
    });
  } else {
    $.post(
      urlBase + "fieldValue?name=" + name + "&value=" + value,
      body,
      function (data) {
        if (data.name != null) {
          $("#status").html("Set " + name + ": " + data.name);
        } else {
          $("#status").html("Set " + name + ": " + data);
        }
      }
    );
  }
}

function delayPostValue(name, value) {
  clearTimeout(postValueTimer);
  postValueTimer = setTimeout(function () {
    postValue(name, value);
  }, 300);
}

function postColor(name, value) {
  $("#status").html(
    "Setting " +
      name +
      ": " +
      value.r +
      "," +
      value.g +
      "," +
      value.b +
      ", please wait..."
  );

  var body = { name: name, r: value.r, g: value.g, b: value.b };

  $.post(
    urlBase + name + "?r=" + value.r + "&g=" + value.g + "&b=" + value.b,
    body,
    function (data) {
      $("#status").html("Set " + name + ": " + data);
    }
  ).fail(function (textStatus, errorThrown) {
    $("#status").html("Fail: " + textStatus + " " + errorThrown);
  });

  if (name === "solidColor") {
    const option = $("#input-pattern option:contains('Solid Color')")[0];
    $("#input-pattern").val(option.index);
  }
}

function delayPostColor(name, value) {
  clearTimeout(postColorTimer);
  postColorTimer = setTimeout(function () {
    postColor(name, value);
  }, 300);
}

function componentToHex(cNumber) {
  var hex = cNumber.toString(16);
  return hex.length == 1 ? "0" + hex : hex;
}

function rgbToHex(value) {
  const rgb = value.split(",");
  return (
    "#" +
    componentToHex(parseInt(rgb[0])) +
    componentToHex(parseInt(rgb[1])) +
    componentToHex(parseInt(rgb[2]))
  );
}

function hexToRgb(hex) {
  var result = /^#?([a-f\d]{2})([a-f\d]{2})([a-f\d]{2})$/i.exec(hex);
  return result
    ? {
        r: parseInt(result[1], 16),
        g: parseInt(result[2], 16),
        b: parseInt(result[3], 16),
      }
    : null;
}

function rgbToComponents(rgb) {
  var components = {};

  rgb = rgb.match(/^rgb\((\d+),\s*(\d+),\s*(\d+)\)$/);
  components.r = parseInt(rgb[1]);
  components.g = parseInt(rgb[2]);
  components.b = parseInt(rgb[3]);

  return components;
}
