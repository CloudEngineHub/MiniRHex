const char *WEBPAGE = R"html-delimiter(
<!doctype html>
<head>
  <title>MiniRHex</title>
  <style>
    .buttons {
      display:inline-grid; grid-template-columns: 25% 25% 25% 25%;
      grid-template-rows: 25% 25% 25% 25%; width: 400px; height: 400px;
    }
    button {
      font-size: 12pt; border-width: 4px;
    }
    select, p {
      font-size: 12pt;
    }
    .group {
      display: flex;
      gap: 10px;
      margin-left: 20px;
    }
    @media (max-aspect-ratio: 1) {
      .buttons {
        width: 90vw; height: 90vw; padding: 5vw;
      }
      button {
        font-size: 4vw; border-width: 0.5vw;
      }
      select, p {
        font-size: 4vw;
      }
      input {
        width: 60vw;
      }
      input::-webkit-slider-thumb {
        width: 7vw;
        height: 7vw;
      }
    }
  </style>
</head>

<body>
  <div class=buttons>
  <b></b><button onclick="window.location.href='?key=r';">Run<br>(r)</button>
  <b></b><button onclick="window.location.href='?key=z';">Reboot<br>(z)</button>
  <b></b><button onclick="window.location.href='?key=w';">Forward<br>(w)</button><b></b><b></b>
  <button onclick="window.location.href='?key=a';">Left<br>(a)</button>
  <button onclick="window.location.href='?key=q';">Stop<br>(space)</button>
  <button onclick="window.location.href='?key=d';">Right<br>(d)</button><b></b>
  <b></b><button onclick="window.location.href='?key=s';">Reverse<br>(s)</button><b></b><b></b></div>
  <br><br><hr><br>

  <button onclick="updateParams(false)">Upload</button>
  <button onclick="updateParams(true)">Save</button>
  <button onclick="loadParams()">Load</button>
  <button onclick="resetParams()">Reset</button>
  <br><br><hr>

  <p><b>Gait</b>: The pattern of how the legs move.</p>
  <div class="group">
    <select id="gait-select" onchange="updateGait()">
      <option value="bipod">Bipod</option>
      <option value="tripod">Tripod</option>
      <option value="tetrapod">Tetrapod</option>
      <option value="wave">Wave</option>
      <option value="ripple">Ripple</option>
      <option value="metachronal">Metachronal</option>
      <option value="bound">Bound</option>
      <option value="pronk">Pronk</option>
    </select>
    <p id="gait-label"></p>
  </div>
  <br><hr>

  <p><b>Period:</b> The amount of time it takes to move all of the legs.</p>
  <div class="group">
    <input id='period-select' type='range' min='0.5' max='4' step='0.1' oninput="updateSlider('period')">
    <p id='period-label'></p>
  </div>
  <br><hr>

  <p><b>Duty Factor:</b> The amount of time each leg is on the ground.</p>
  <div class="group">
    <input id='duty-select' type='range' min='0' max='100' step='5' oninput="updateSlider('duty')">
    <p id='duty-label'></p>
  </div>
  <br><hr>

  <p><b>Sweep Angle:</b> The amount each leg rotates while on the ground.</p>
  <div class="group">
    <input id='sweep-select' type='range' min='0' max='360' step='5' oninput="updateSlider('sweep')">
    <p id='sweep-label'></p>
  </div>
  <br><hr>

  <p><b>Down Angle:</b> The average angle of each leg while on the ground.</p>
  <div class="group">
    <input id='down-select' type='range' min='0' max='360' step='5' oninput="updateSlider('down')">
    <p id='down-label'>Test</p>
  </div>
</body>

<script>
  window.onload = function(evt) {
    var GAIT = "tripod"
    var PERIOD = 2
    var DUTY = 60
    var SWEEP = 30
    var DOWN = 0
    updateGait((GAIT))
    updateSlider("period", (PERIOD))
    updateSlider("duty", (DUTY))
    updateSlider("sweep", (SWEEP))
    updateSlider("down", (DOWN))
  }

  document.onkeydown = function(evt) {
    evt = evt || window.event;
    if (evt.key === " ") {
      window.location.href = "?key=q";
    } else {
      window.location.href = "?key=" + evt.key;
    }
  };

  function updateGait(value) {
    const select = document.getElementById("gait-select");
    const label = document.getElementById("gait-label");
    const descriptions = {
      "bipod": "Move the legs on the left side, then the legs on the right side.",
      "tripod": "Move three legs at a time in a triangle shape.",
      "tetrapod": "Move two legs at a time.",
      "wave": "Move one leg at a time.",
      "ripple": "Move one leg at a time, with each leg starting right after the one before it.",
      "metachronal": "Move the back legs first, then the middle legs, then the front legs.",
      "bound": "Move the back legs first, then the front legs.",
      "pronk": "Move all six legs at the same time."
    };
    if (arguments.length === 1) {
      select.value = value
    }
    label.innerHTML = descriptions[select.value];
  };

  function updateSlider(name, value) {
    const slider = document.getElementById(name + "-select")
    const label = document.getElementById(name + "-label")
    const units = {
      "period": " seconds",
      "sweep": " degrees",
      "down": " degrees",
      "duty": "%"
    }
    if (arguments.length === 2) {
      slider.value = value
    }
    label.innerHTML = slider.value + units[name]
  };

  function updateParams(save=false) {
    var input = save ? "?save=true&" : "?";
    params = ["gait", "period", "sweep", "down", "duty"]
    for (let i = 0; i < params.length; i++) {
        input += params[i] + "=" + document.getElementById(params[i] + "-select").value + "&";
    }
    window.location.href = input.slice(0, -1);
  }

  function loadParams() {
    window.location.href = "?load=true"
  }

  function resetParams() {
    window.location.href = "?reset=true"
  }
</script>
)html-delimiter";
