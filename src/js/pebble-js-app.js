Pebble.addEventListener("ready",
  function (e) {
      console.log("ready");
  }
);

Pebble.addEventListener("appmessage",
  function (e) {
    console.log("message");
    fetchData();
  }
);

function fetchData() {
  var response;
  var req = new XMLHttpRequest();
  req.open('GET', "http://thegrebs.com/~michael/.thermostat.pl?token=" + Pebble.getAccountToken(), true);
  req.onload = function(e) {
    if (req.readyState == 4) {
      if(req.status == 200) {
        console.log(req.responseText);
        response = JSON.parse(req.responseText);
        if (response.temp) {
          Pebble.sendAppMessage(response);
        }
      } else {
        console.log("Request returned error code " + req.status.toString());
      }
    }
  };
  req.send();
}
